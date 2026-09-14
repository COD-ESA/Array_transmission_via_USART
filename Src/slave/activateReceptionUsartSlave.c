/*activateReceptionUsartSlave.c*/
/*Функция будет вызываться по прерыванию от приемника usart*/
#include "main.h"

#define MESSAGE "Get byte : "
#define MESSAGE_WRONG "Get byte ERROR!!!: "

void receptionDataWrong(uint8_t data)
{
    // USART1->CR1 &= ~USART_CR1_RE;
    led_On();

        const char * str = MESSAGE_WRONG;

    while(*str){
        while(!(USART1 -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USART1 -> DR = *str++;
    }
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = data;
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}    
    USART1 -> DR = '\r';
    while(!(USART1 -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USART1 -> DR = '\n';

    // USART1->CR1 |= USART_CR1_RWU | USART_CR1_RE;
}

/*
Функция receptionDataUSARTx кроме получения в аргументах обычных переменных, получает так же и указатель
на функцию FUNC func, что делает гибче обработку данных в функции receptionDataUSARTx. 
Так же принимает указатель на модуль USART_TypeDef *USARTx, чтобы избежать жесткой привязки 
функции к одному модулю USART. 
*/
void receptionDataUSARTx(FUNC func, USART_TypeDef *USARTx,uint32_t data, const char *message)
{   
    char out_buf[12]; // Буфер для хранения строки с числом
    char *str_value = func(data, out_buf);

    while(*message){
        while(!(USARTx -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USARTx -> DR = *message++;
    }
    while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}

    while(*str_value){
        while(!(USARTx -> SR & USART_SR_TXE)){ /*жду освобождения передатчика*/}
        USARTx -> DR = *str_value++;
    }

    while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}    
    USARTx -> DR = '\r';
    while(!(USARTx -> SR & USART_SR_TXE)){/*жду освобождения передатчика*/}
    USARTx -> DR = '\n';
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