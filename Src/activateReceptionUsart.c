/*activateReceptionUsart.c*/
/*Функция будет вызываться по прерыванию от приемника usart*/
#include "main.h"

#define MESSAGE "Get byte : "
#define MESSAGE_WRONG "Get byte ERROR!!!: "


static void usartSend_address(USART_TypeDef *USARTx, uint8_t addr)
{   
   // Жду готовности передатчика
   while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
   // Передаю адрес с 1-ей в 9-ом бите
   uint16_t result = (0x100 | (addr & 0x0f));//переменная только для отладки программы,после готовности прошивки удалить и писать на прямую в USARTx->DR
   USARTx->DR = result;
}

// static void usartSend_data(USART_TypeDef *USARTx, uint8_t data)
// {
//     // Жду готовности передатчика
//     while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
//     // Передаю адрес с 1-ей в 9-ом бите
//     USARTx->DR = (data & 0xff);    
// }

void receptionData(uint8_t data, const char *message)
{   
    led_On();

    while(*message){
        while(!(USART1 -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USART1 -> DR = ((uint16_t)*message++) & 0x00ff;
    }
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = data;
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}    
    USART1 -> DR = '\r';
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = '\n';
}

void receptionDataWrong(uint8_t data, const char *message_wrong)
{
    led_On();

    while(*message_wrong){
        while(!(USART1 -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USART1 -> DR = *message_wrong++;
    }
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = data;
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}    
    USART1 -> DR = '\r';
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = '\n';
}

/*
Функция receptionDataUSARTx кроме получения в аргументах обычных переменных, получает так же и указатель
на функцию FUNC func, что делает гибче обработку данных в функции receptionDataUSARTx. 
Так же принимает указатель на модуль USART_TypeDef *USARTx, чтобы избежать жесткой привязки 
функции к одному модулю USART. 
*/
void receptionDataUSARTx(FUNC func, uint8_t address, USART_TypeDef *USARTx, uint32_t data, const char *message)
{   
    usartSend_address(USARTx, address);

    char out_buf[12]; // Буфер для хранения строки с числом
    char *str_value = func(data, out_buf);
   
    while(*message){
        while(!(USARTx -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        uint8_t result = (uint8_t)*message++;
        USARTx->DR = result;
    }

    while(*str_value){
        while(!(USARTx -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USARTx->DR = (uint8_t)*str_value++;
    }

    while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USARTx->DR = (uint8_t)'\r';

    while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USARTx->DR = (uint8_t)'\n';
}

char *send_uint32(uint32_t value, char *out_buf)
{
    // Буфер должен быть размером минимум 11 байт
    char *p = &out_buf[11];
    *p = '\0';

    do
    {
        *--p = (char)('0' + (value % 10));
        value /= 10;
    }
    while(value);

    return p; // Возвращает указатель на начало сформированной строки внутри out_buf
}