/*ManualSystemInit.c*/
/* Manual System Initialization */

#include "main.h"


void ManualSystemInit(void)
{   
    uint32_t volatile timeout;

    /*1 вкл тактирование PWR для того чтобы было возможно установить режим VOS*/
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;

    /*2 set VOS = scale_X, wait VOSRDY*/
    PWR -> CR |= PWR_CR_VOS; // Устанавливаю режим  (Scale 1) <=100MHz (11)
    (void)PWR -> CR;

    /* 3. ПРОВЕРКА: Считываем из того же регистра CR! */
    if ((PWR->CR & PWR_CR_VOS) != PWR_CR_VOS)
    {
        /* Сюда мы попадем, только если регистр физически не записался (например, сбой питания) */
        Default_Handler();
    }
    
    timeout = 10000;
    while (!(PWR->CSR & PWR_CSR_VOSRDY) && --timeout);

    /*4 выбираю источник тактирования*/
    RCC -> CR &= ~RCC_CR_HSEBYP; // Очищаем бит HSEBYP, чтобы использовать внешний кварц
    RCC -> CR |= RCC_CR_HSEON; // Включаем внешний кварц (HSE)

    timeout = 10000;
    while( (RCC -> CR & RCC_CR_HSERDY) == 0)
    {
      if (--timeout == 0)
      {
       Default_Handler(); // Infinite loop to indicate failure
      }  
      __NOP(); // Wait for HSE to be ready
    }

    InitFlash(); // Настройка Flash памяти

    /*5*/
    RCC -> CR &= ~RCC_CR_PLLON; // Выключаем PLL перед настройкой
    while (RCC->CR & RCC_CR_PLLRDY); // жду выключения PLL

    RCC -> PLLCFGR = (
      RCC_PLLCFGR_PLLSRC_HSE | // выбор источника тактирования от HSE
      (25U << RCC_PLLCFGR_PLLM_Pos) | /*M 25*/
      (192U << RCC_PLLCFGR_PLLN_Pos) | /*N 192*/
      (0U << RCC_PLLCFGR_PLLP_Pos) /*P 2*/
      ); 
    
    uint32_t rcc_cfgr = RCC -> CFGR;

    /*ON PLL*/    
    RCC -> CR |= RCC_CR_PLLON; // Включаем PLL
  
    timeout = 10000;    
    while( (RCC -> CR & RCC_CR_PLLRDY) == 0)
    {
      if (--timeout == 0)
      {
       Default_Handler(); // Infinite loop to indicate failure
      }  
      __NOP(); // Wait for PLL to be disabled
    }   

    /*6*/
    rcc_cfgr &= ~RCC_CFGR_HPRE; /* AHB prescaler = HCLK = SYSCLK*/
    rcc_cfgr &= ~RCC_CFGR_SW;  /*Очищаем биты выбора источника SYSCLK*/

    rcc_cfgr |= (
      RCC_CFGR_PPRE1_DIV2 | /*APB1 prescaler = 2 (48 MHz)*/
      RCC_CFGR_PPRE2_DIV1 | /*APB2 prescaler = 1 (96 MHz)*/
      RCC_CFGR_SW_PLL
    );

    RCC -> CFGR = rcc_cfgr;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    // Implementation for manual system initialization
}
