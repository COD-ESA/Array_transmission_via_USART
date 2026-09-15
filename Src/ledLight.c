/*
1 светодиод висит на PC13
адрес порта С = 0x4002 0800 - 0x4002 0BFF
порт С питается от шины AHB1
*/ 
#include "main.h"

/*включаю тактирование порта С и настраиваю на выход*/
void config_clock_and_output_PC(void)
{
    /*затактировать порт C*/   
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
    (void)RCC->AHB1ENR;
    
    /*НАСТРАИВАЮ ПОРТ С НА ВЫХОД*/
    GPIOC ->MODER &= ~(GPIO_MODER_MODER13);
    GPIOC ->MODER |= (GPIO_MODER_MODER13_0);

    GPIOC ->OTYPER &= ~(GPIO_OTYPER_OT13);

    GPIOC ->PUPDR &= ~(GPIO_PUPDR_PUPD13);
    GPIOC ->PUPDR |= GPIO_PUPDR_PUPD13_1;

    /*сброс 29-го бита*/
    // GPIOC ->BSRR = (GPIO_BSRR_BR13); //лед светится

    /*установка 29 бита*/
    GPIOC ->BSRR = GPIO_BSRR_BS13; //лед не светится
}

/*ВКЛ/ВЫКЛ 13-ГО БИТА В ПОРТУ PC - МОРГАНИЕ ЛЕД*/
void toglle_pc13(void)
{
    if(GPIOC ->ODR & GPIO_ODR_OD13)
    {
      GPIOC ->BSRR = GPIO_BSRR_BR13;  
    } else
    {
      GPIOC ->BSRR = GPIO_BSRR_BS13;   
    }
}

void toggle_pc13_mod(void)
{
    GPIOC ->ODR ^= GPIO_ODR_OD13;
}

void led_On(void)
{   
  //подаю ноль на 13 пин порта С чтобы включить лед
  GPIOC ->BSRR = GPIO_BSRR_BR13;
  /*жду секунду*/
  __IO uint32_t timeout = 25000000; // 25000000
  while(timeout){
  timeout--;
  }
  /*подачей  1 выключаю лед*/
  GPIOC ->BSRR = GPIO_BSRR_BS13;
  timeout = 25000000; // 25000000
  while(timeout){
    timeout--;
  }
}

/*Функция просто включает лед для визуального контроля что условие отработало*/
void ledPowerOn(void)
{
  GPIOC ->BSRR = GPIO_BSRR_BR13;  
}