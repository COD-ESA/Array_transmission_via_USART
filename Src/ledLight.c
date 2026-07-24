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
    GPIOC ->BSRR = (GPIO_BSRR_BR13);
}

/*ВКЛ/ВЫКЛ 13-ГО БИТА В ПОРТУ PC - МОРГАНИЕ ЛЕД*/
void toglle_pc13(void)
{
    if(GPIOC ->IDR & GPIO_IDR_ID13)
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