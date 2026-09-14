/*initUsartSlave.c*/
/*
настройка PA9 на полудуплексный режим
*/
#include "main.h"

/*
PA11 -  USART1_CTS разрешение на отправку
PA12 -  USART1_RTS запрос на отправку
PA9 - TX
PA10 - RX
*/

/*
USART1 сидит на шине APB2 максимальная частота шины 100МГц.
Вывод PA9 настроен в Open-Drain (Push-Up) для подключения.
PA9 - TX (Получает данные от мастера) USART1
*/
void initUSART1_Slave(uint8_t slave_addr)
{
    // 1. Тактирование порта А
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    // 2. Тактирование модуля USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    (void)RCC->APB2ENR;

    // 3. GPIO PA9 (TX) -> Alternate Function AF7, Push-Up, High-Speed
    GPIOA->MODER &= ~GPIO_MODER_MODER9;
    GPIOA->MODER |= GPIO_MODER_MODER9_1; // AF

    // 4. 
    GPIOA->OTYPER |= GPIO_OTYPER_OT9; // Open-Drain

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD9_0; // Pull-Up

    // 5. 
     GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9; // High-Speed

    // 6. 
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos); // AF7

    // 7. Отключаю усарт
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    // 8. Настройка скорости передачи байт
    USART1->BRR = ((100000000UL + 9600/2) / 9600);

    // 9. CR1: Включаем 9-битный режим (M=1), БЕЗ четности (PCE=0), прерывание RXNE, RX, TX, UE
    uint32_t usart1_cr1 = USART1->CR1;
    usart1_cr1 =   
            USART_CR1_RXNEIE    | /*разрешить прерывание*/
            USART_CR1_RE        | /*включить приемник*/
            USART_CR1_TE        | /*включение передатчика*/
            USART_CR1_M         | /*режим кадра 9 бит*/
            USART_CR1_WAKE      | /*пробуждение модуля усарт по метке адреса*/
            USART_CR1_RWU       | /*приемник переведен в режим молчания*/
            USART_CR1_UE;         /*включить модуль усарт*/  


    // 10. Настройка CR3: включаем Half-Duplex (Single-Wire) режим
    USART1->CR3 |= USART_CR3_HDSEL;
    (void)USART1->CR3;
    
    // 11. Установка адреса МК (только 4 младших бита 0x0..0xF)
    USART1->CR2 |= ((slave_addr & 0x0F) << USART_CR2_ADD_Pos);
    (void)USART1->CR2;

    USART1->CR1 = usart1_cr1;            
}

// PA2 - TX (выводит данные на ПК) USART2
void initUsart2_Slave(void)
{
    // 1. Тактирование порта А
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    // Тактирование модуля USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    // 2. GPIO PA3 (RX) , GPIO PA2 (TX) -> Alternate Function AF7, Push-Up, High-Speed
    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->MODER |= GPIO_MODER_MODER2_1; // AF

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT2; // Output push-pull  
    
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2; // No Pull-Up/Pull-down

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2; // High-Speed

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos); // AF7

    // 3. Отключаем USART перед конфигурацией
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // 4. Скорость Baud Rate (убедись, что APB2 точно равен 100 МГц!)

    USART2->BRR = ((50000000UL + 9600/2) / 9600);

    // 6. CR1: Включаем 8-битный режим (M=0), БЕЗ четности (PCE=0), прерывание RXNE, RX, TX, UE            
    uint32_t usart2_cr1 = USART2->CR1;
    usart2_cr1 = 
        // USART_CR1_TXEIE     | /*разрешить прерывание*/
        USART_CR1_TE        | /*включение передатчика*/
        // USART_CR1_M         | /*режим кадра 9 бит*/
        USART_CR1_UE;         /*включить модуль усарт*/  

    USART2->CR1 = usart2_cr1;
}