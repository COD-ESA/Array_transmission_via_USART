/*initUsart.c*/

#include "main.h"

#define APB2_CLK 100000000
#define APB1_CLK 50000000UL

/*конфигурирую пин PA10 на прием данных по USART*/
/*
PA11 -  USART1_CTS разрешение на отправку
PA12 -  USART1_RTS запрос на отправку
*/
/*
Через модуль УСАРТ1 (PA9 - TX) ведущий отправляет
данные из флеш памяти на ведомые МК
*/
void initUsart1(void)
{
    /* 1. вкл тактирование порта А*/
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    /* 2. настроить пин А10(RX) A9(TX)*/

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT10; // PP
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD10; // очистка мусора
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD10_0; //01 PP +PU
    GPIOA->OTYPER &= GPIO_OTYPER_OT9; // Open-Drain
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9; // очистка мусора
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD9_0; //01 PP + PU 


    /* 3. настроить пин А10 A9 на альтернативную функцию AF7 - 0111*/
    GPIOA->MODER &= ~GPIO_MODER_MODER9; // очистка мусора 
    GPIOA->MODER |= GPIO_MODER_MODER9_1; // 10 

    GPIOA->MODER &= ~GPIO_MODER_MODER10; // очистка мусора 
    GPIOA->MODER |= GPIO_MODER_MODER10_1; // 10

    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
    GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL9_0 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2); //111

    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10;
    GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2); //111

    /* 4. настроить скорость Max*/
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED10;

    /* 5. включить тактирование USART*/
    RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
    (void)RCC -> APB2ENR;

    /* 6. отключить перед настройкой USART*/
    USART1 -> CR1 &= ~USART_CR1_UE;
    
    uint32_t usart_cr1 = USART1->CR1;

    /* 5. Настроить скорость */
    /*Стандартный режим OVER8 = 0*/
    USART1 -> BRR = ((APB2_CLK + 9600/2) / 9600);   // 9600 бод при 100 МГц

    /* 6. Настройка длинны слова 9 бит в пакете*/
    usart_cr1 |= USART_CR1_M;

    /* 7. Четное количество единиц*/

    // usart_cr1 |=USART_CR1_PCE;
    // usart_cr1 &= ~USART_CR1_PS;

    /* 8. разрешить прерывание */
    // usart_cr1 |= USART_CR1_RXNEIE;
    // USART1 -> CR1 |= USART_CR1_TXEIE;

    /* 9. Включить приёмник */
    // usart_cr1 |=  USART_CR1_RE;

    /* 10. Включить передатчик */
    usart_cr1 |=  USART_CR1_TE;

    // 10. Настройка CR3: включаем Half-Duplex (Single-Wire) режим
    USART1->CR3 |= USART_CR3_HDSEL;
    /* 11. Включить USART*/
    USART1->CR1 = usart_cr1;
    USART1 -> CR1 |= USART_CR1_UE;
    (void)USART1 -> CR1;
}


void initUsart2(void)
{
    // 1. Тактирование порта А
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    // 2. GPIO PA2 (TX) , GPIO PA3 (RX) -> Alternate Function AF7, Push-Up, High-Speed
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->MODER |= GPIO_MODER_MODER2_1; // AF

     GPIOA->OTYPER &= ~GPIO_OTYPER_OT2; // Output push-pull
     
     GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2; // No Pull-Up/Pull-down

     GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2; // High-Speed

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos); // AF7

    uint32_t usart2_cr1 = USART2->CR1;

    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    USART2->BRR = ((APB1_CLK + 9600/2) / 9600);

    usart2_cr1 = 
        // USART_CR1_TXEIE     | /*разрешить прерывание*/
        USART_CR1_TE        | /*включение передатчика*/
        // USART_CR1_M         | /*режим кадра 9 бит*/
        USART_CR1_UE;         /*включить модуль усарт*/ 

    USART2->CR1 = usart2_cr1;        
}                
