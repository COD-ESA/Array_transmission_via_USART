/*specialSettingUsart.c*/
/*файл отвечает за дополнительные настройки модуля usart ведомого микроконтроллера*/

#include "main.h"

void USART1_Init_Slave(uint8_t slave_addr)
{
    USART1->CR1 &= ~USART_CR1_UE;
    (void)USART1 -> CR1;  
    // 1. 9-битный кадр, метод пробуждения по метке адреса WAKE=1, прерывание RXNE.
    USART1->CR1 = USART_CR1_M | USART_CR1_WAKE | USART_CR1_RXNEIE;
    
    // 2. Настройка CR3: включаем Half-Duplex (Single-Wire) режим
    USART1->CR3 |= USART_CR3_HDSEL;

    // 3. Установка адреса МК (только 4 младших бита 0x0..0xF)
    USART1->CR2 = (USART1->CR2 & ~USART_CR2_ADD) | ((slave_addr & 0x0F) << USART_CR2_ADD_Pos);

    // 4. Переводим приемник в режим ожидания своего адреса
    USART1->CR1 |= USART_CR1_RWU;

    // 5. Включение RX, и модуля USART
    USART1->CR1 |= USART_CR1_RE | USART_CR1_UE;
    (void)USART1 -> CR1;
}
