/*readValueRegistr.c*/

#include "main.h"

void send_AHB1ENR_hex(void) {

    uint32_t val = RCC->AHB1ENR;
    const char hex_chars[] = "0123456789ABCDEF";
    
    // Префикс строки
    const char prefix[] = "RCC->AHB1ENR = 0x";
    for (int i = 0; prefix[i] != '\0'; i++) {
        while (!(USART2->SR & USART_SR_TXE));
        USART2->DR = (uint8_t)prefix[i];
    }

    // Отправляем 8 HEX-цифр (начиная со старшего полубайта)
    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (val >> (i * 4)) & 0x0F; // выделяем 4 бита
        while (!(USART2->SR & USART_SR_TXE));
        USART2->DR = (uint8_t)hex_chars[nibble];
    }

    // Перевод строки
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = '\r';
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = '\n';
}