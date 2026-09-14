/*mainSlave.c*/

#include "main.h"
#define N 65536
#define ADDRESS_USART_SLAVE 0x08
extern void jump_initUsart(void *func, uint8_t);
extern void initUSART1_Slave(uint8_t slave_addr);
extern void initUsart2_Slave(void);
extern void config_mco1(void);
extern void config_clock_and_output_PC(void);//включение тактирования порта и настройка вывода PC13
extern void send_AHB1ENR_hex(void);
extern void readORwrite_of_Flash(const _Bool status, const uint32_t start_address, const uint32_t quantity_byte, uint32_t sector, uint32_t (*func)(uint32_t));
extern void receptionDataWrong(uint8_t data);
extern char *send_uint32(uint32_t value, char *out_buf);
extern void receptionDataUSARTx(FUNC func, USART_TypeDef *USARTx,uint32_t data, const char *message);
extern void ledPowerOn(void);

static volatile uint8_t usart1_rxne_flag = 0;
static volatile uint16_t rx_data_buffer = 0;

int main(void)       
{  
   __enable_irq();// включение прерываний
   config_clock_and_output_PC();// тактирование порта С и настройка вывода С13 
   // config_mco1();// Вывод тактового сигнала на PA8
   jump_initUsart(initUSART1_Slave, ADDRESS_USART_SLAVE); // настройка УСАРТ1
   jump_initUsart(initUsart2_Slave, 0); // настройка УСАРТ2
   Enable_IRQn();

   while(1) 
   {  
      if(usart1_rxne_flag) {
         usart1_rxne_flag = 0; //сброс флага
         // Ждем освобождения передатчика USART2
        while(!(USART2->SR & USART_SR_TXE));
         // Отправляем байт на ПК (только 8 младших бит)
         USART2->DR = (uint8_t)rx_data_buffer;

         /*
         Если получили символ "НОВОЙ СТРОКИ"
         значит ведомый получил посылку полностью
         и модуль USART1 перевести в режим mute.
         */
         if((uint8_t)rx_data_buffer == '\n'){
            while(USART1->SR & USART_SR_RXNE){
               (void)USART1->DR;
            };
            USART1->CR1 |= USART_CR1_RWU;
         }
      }
   }
}

/*все прерывания включаю в файле IRQn.c*/
void USART1_Handler(void)
/*
Обрабатывает получение данных по usart на PA9(TX)
почему данный получаю через TX? Потому что включен 
режим Half-Duplex (Single-Wire) режим
*/
{  
   uint32_t sr = USART1->SR;
   if(sr & (USART_SR_NE | USART_SR_FE | USART_SR_ORE)) {
      /*
      * код обработки получения некорректных данных
      * Игнорируем байт или логируем ошибку
      */
      __IO uint8_t data = (uint8_t)USART1->DR;
      (void)data;
      return;
   }

   if(sr & USART_SR_RXNE) {
      // Чтение DR сбрасывает бит RXNE!
      uint16_t verification_data = (uint16_t)(USART1->DR & 0x01ff);
      if(verification_data & 0x0100){
         /*
         пришела адресная посылка её нужно убрать из потока получаемых данных
         поэтому ни чего не делаем.
         */
        return;
      } 
      /*
      Пришли данные для обработки
      */
      else{
         usart1_rxne_flag = 1;
         rx_data_buffer = verification_data;
         return;
      }

   }

   if(sr & USART_SR_IDLE) {
      volatile uint32_t dummy = USART1->DR; 
      (void)dummy;
      // USART1->CR1 |= USART_CR1_RWU;
      return;
   }
}