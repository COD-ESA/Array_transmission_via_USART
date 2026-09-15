/*mco1.c */

#include "main.h"

void config_mco1(void)
{ 
    /*P - MCO1*/
    /*MCO1 and MCO2: these pins have to be configured in alternate function mode*/
    /*1. затактировать порт А*/
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    (void)RCC->AHB1ENR;     
    
    /*2. Настроить PA8 на альтернативную функцию MCO1*/
    GPIOA->MODER &= ~(GPIO_MODER_MODER8); // Очистка    
    GPIOA->MODER |= (GPIO_MODER_MODER8_1); // Установка альтернативной функции (10)

    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8);
    // GPIOA->AFR[1] |= (0U << GPIO_AFRH_AFSEL8_Pos); 

    /*3. Настроить MCO1 на вывод HSE с делителем 4*/
    RCC->CFGR &= ~(RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE); // Очистка битов MCO1 и MCO1PRE
    // RCC->CFGR |= (RCC_CFGR_MCO1_1); // Установка MCO1 = HSE, делитель = 0 
    // RCC->CFGR |= (RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_1); // Установка MCO1 = PLL, делитель = 4  
    RCC->CFGR |= RCC_CFGR_MCO1; // Установка MCO1 = PLL, делитель = 0  
    
}