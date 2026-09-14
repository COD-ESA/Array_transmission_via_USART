/*IRQn.c*/

#include "main.h"

void Enable_IRQn(void)
{
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
}
