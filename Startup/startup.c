/*starup.c*/

#include "main.h"

#define WEAK_HANDLER(name) void name(void) __attribute__ ((weak, alias("Default_Handler")))
/*блок внешних секций объявленных в файле линкера*/
extern uint32_t _estack; /*Адрес вершины стека*/
extern uint32_t _sidata; /*Начало секции .data во flash откуда копировать*/
extern uint32_t _sdata; /*Начало секции .data в RAM куда копировать*/
extern uint32_t _edata; /*Конец секции .data в RAM*/
extern uint32_t _sbss; /*Начало секции .bss в RAM*/
extern uint32_t _ebss; /*Конец секции .bss в RAM*/

/*прототипы системных функций*/
void Reset_Handler(void);
void Default_Handler(void);
void ManualSystemInit(void);
int main(void);


/* Прототипы системных исключений ядра (Индекс в vector[], IRQ не имеют) */
WEAK_HANDLER(Reset_Handler);               /* Индекс 1 (Вектор сброса) */
WEAK_HANDLER(NMI_Handler);                 /* Индекс 2 */
WEAK_HANDLER(Hard_Handler);                /* Индекс 3 */
WEAK_HANDLER(MemManage_Handler);           /* Индекс 4 */
WEAK_HANDLER(BusFault_Handler);            /* Индекс 5 */
WEAK_HANDLER(UsageFault_Handler);          /* Индекс 6 */
/* Индексы 7, 8, 9, 10 зарезервированы (нули в таблице) */
WEAK_HANDLER(SVCall_Handler);              /* Индекс 11 */
WEAK_HANDLER(DebugMonitor_Handler);        /* Индекс 12 */
/* Индекс 13 зарезервирован (ноль в таблице) */
WEAK_HANDLER(PendSV_Handler);              /* Индекс 14 */
WEAK_HANDLER(Systick_Handler);             /* Индекс 15 */

/* Прототипы внешних прерываний периферии (Индекс в vector[] / Аппаратный номер IRQ) */
WEAK_HANDLER(WWDG_Handler);                /* Индекс 16 / IRQ 0  */
WEAK_HANDLER(EXTI16_PVD_Handler);          /* Индекс 17 / IRQ 1  */
WEAK_HANDLER(EXTI21_TAMP_STAMP_Handler);   /* Индекс 18 / IRQ 2  */
WEAK_HANDLER(EXTI22_RTC_WKUP_Handler);     /* Index 19 / IRQ 3  */
WEAK_HANDLER(FLASH_Handler);               /* Индекс 20 / IRQ 4  */
WEAK_HANDLER(RCC_Handler);                 /* Индекс 21 / IRQ 5  */
WEAK_HANDLER(EXTI0_Handler);               /* Индекс 22 / IRQ 6  */
WEAK_HANDLER(EXTI1_Handler);               /* Индекс 23 / IRQ 7  */
WEAK_HANDLER(EXTI2_Handler);               /* Индекс 24 / IRQ 8  */
WEAK_HANDLER(EXTI3_Handler);               /* Индекс 25 / IRQ 9  */
WEAK_HANDLER(EXTI4_Handler);               /* Индекс 26 / IRQ 10 */
WEAK_HANDLER(DMA1_Stream0_Handler);        /* Индекс 27 / IRQ 11 */
WEAK_HANDLER(DMA1_Stream1_Handler);        /* Индекс 28 / IRQ 12 */
WEAK_HANDLER(DMA1_Stream2_Handler);        /* Индекс 29 / IRQ 13 */
WEAK_HANDLER(DMA1_Stream3_Handler);        /* Индекс 30 / IRQ 14 */
WEAK_HANDLER(DMA1_Stream4_Handler);        /* Индекс 31 / IRQ 15 */
WEAK_HANDLER(DMA1_Stream5_Handler);        /* Индекс 32 / IRQ 16 */
WEAK_HANDLER(DMA1_Stream6_Handler);        /* Индекс 33 / IRQ 17 */
WEAK_HANDLER(ADC_Handler);                 /* Индекс 34 / IRQ 18 */
WEAK_HANDLER(EXTI9_5_Handler);             /* Индекс 35 / IRQ 23 */ /* Внимание: в STM32F411 тут пропуск IRQ 19-22, их нет в этой таблице */
WEAK_HANDLER(TIM1_BRK_TIM9_Handler);       /* Индекс 36 / IRQ 24 */
WEAK_HANDLER(TIM1_UP_TIM10_Handler);       /* Индекс 37 / IRQ 25 */
WEAK_HANDLER(TIM1_TRG_COM_TIM11_Handler);  /* Индекс 38 / IRQ 26 */
WEAK_HANDLER(TIM1_CC_Handler);             /* Индекс 39 / IRQ 27 */
WEAK_HANDLER(TIM2_Handler);                /* Индекс 40 / IRQ 28 */
WEAK_HANDLER(TIM3_Handler);                /* Индекс 41 / IRQ 29 */
WEAK_HANDLER(TIM4_Handler);                /* Индекс 42 / IRQ 30 */
WEAK_HANDLER(I2C1_EV_Handler);             /* Индекс 43 / IRQ 31 */
WEAK_HANDLER(I2C1_ER_Handler);             /* Индекс 44 / IRQ 32 */
WEAK_HANDLER(I2C2_EV_Handler);             /* Индекс 45 / IRQ 33 */
WEAK_HANDLER(I2C2_ER_Handler);             /* Индекс 46 / IRQ 34 */
WEAK_HANDLER(SPI1_Handler);                /* Индекс 47 / IRQ 35 */
WEAK_HANDLER(SPI2_Handler);                /* Индекс 48 / IRQ 36 */
WEAK_HANDLER(USART1_Handler);              /* Индекс 49 / IRQ 37 */
WEAK_HANDLER(USART2_Handler);              /* Индекс 50 / IRQ 38 */
WEAK_HANDLER(EXTI15_10_Handler);           /* Индекс 51 / IRQ 40 */ /* Пропуск IRQ 39 */
WEAK_HANDLER(EXTI17_RTC_Alarm_Handler);    /* Индекс 52 / IRQ 41 */
WEAK_HANDLER(EXTI18_OTG_FS_WKUP_Handler);  /* Индекс 53 / IRQ 42 */
WEAK_HANDLER(DMA1_Stream7_Handler);        /* Индекс 54 / IRQ 47 */ /* Пропуск IRQ 43-46 */
WEAK_HANDLER(SDIO_Handler);                /* Индекс 55 / IRQ 49 */ /* Пропуск IRQ 48 */
WEAK_HANDLER(TIM5_Handler);                /* Индекс 56 / IRQ 50 */
WEAK_HANDLER(SPI3_Handler);                /* Индекс 57 / IRQ 51 */
WEAK_HANDLER(DMA2_Stream0_Handler);        /* Индекс 58 / IRQ 56 */ /* Пропуск IRQ 52-55 */
WEAK_HANDLER(DMA2_Stream1_Handler);        /* Индекс 59 / IRQ 57 */
WEAK_HANDLER(DMA2_Stream2_Handler);        /* Индекс 60 / IRQ 58 */
WEAK_HANDLER(DMA2_Stream3_Handler);        /* Индекс 61 / IRQ 59 */
WEAK_HANDLER(DMA2_Stream4_Handler);        /* Индекс 62 / IRQ 60 */
WEAK_HANDLER(OTG_FS_Handler);              /* Индекс 63 / IRQ 67 */ /* Пропуск IRQ 61-66 */
WEAK_HANDLER(DMA2_Stream5_Handler);        /* Индекс 64 / IRQ 68 */
WEAK_HANDLER(DMA2_Stream6_Handler);        /* Индекс 65 / IRQ 69 */
WEAK_HANDLER(DMA2_Stream7_Handler);        /* Индекс 66 / IRQ 70 */
WEAK_HANDLER(USART6_Handler);              /* Индекс 67 / IRQ 71 */
WEAK_HANDLER(I2C3_EV_Handler);             /* Индекс 68 / IRQ 72 */
WEAK_HANDLER(I2C3_ER_Handler);             /* Индекс 69 / IRQ 73 */
WEAK_HANDLER(FPU_Handler);                 /* Индекс 70 / IRQ 81 */ /* Пропуск IRQ 74-80 */
WEAK_HANDLER(SPI4_Handler);                /* Индекс 71 / IRQ 84 */ /* Пропуск IRQ 82-83 */
WEAK_HANDLER(SPI5_Handler);                /* Индекс 72 / IRQ 85 */


typedef void (*ISR_Handler)(void);

const ISR_Handler vector[] = {
    
    /*Системные векторы ядра ARM Cortex-M4*/

    (ISR_Handler)&_estack,
    Reset_Handler,                  /*Вектор сброса*/
    NMI_Handler,                    /*Немаскируемое прерывание Срабатывает при аварийных ситуациях (например, сбой внешнего кварца HSE или аппаратная ошибка питания)*/
    Hard_Handler,                   /*(Аппаратный сбой). Возникает при критических ошибках*/
    MemManage_Handler,              /*Ошибка управления памятью. Срабатывает при нарушении прав доступа MPU*/
    BusFault_Handler,               /*Ошибка шины. Возникает при проблемах с доступом к памяти*/
    UsageFault_Handler,             /*Ошибка выполнения. Срабатывает при неверных операциях ядра*/
    0,0,0,0,                        /*зарезервировано*/ 
    SVCall_Handler,                 /*Supervisor Call. Вызов системной функции через инструкцию SVC. Используется в RTOS*/
    DebugMonitor_Handler,           /*Отладочный монитор.*/
    0,                              /*зарезервировано*/
    PendSV_Handler,                 /*Pendable Service Call. Асинхронный системный вызов. Обычно используется в операционных системах для мягкого переключения задач (планирования), когда ядро не занято обработкой других прерываний.*/
    Systick_Handler,                /*Системный таймер SysTick. Встроенный в ядро 24-битный таймер*/

    /*Специальные прерывания кристалла (ST)*/

    WWDG_Handler,                   /*сторожевой таймер (Window Watchdog). Срабатывает, если программа зависла или не успела "покормить" собаку в строго определенный временной интервал (окно).*/
    EXTI16_PVD_Handler,             /*Детектор программируемого напряжения (PVD). Срабатывает через линию EXTI16, если напряжение питания MCU падает ниже критического порога. Позволяет экстренно сохранить важные данные в EEPROM/Flash.*/
    EXTI21_TAMP_STAMP_Handler,      /*Защита от вскрытия (Tamper) и временная метка (Timestamp). Генерируется часами реального времени (RTC) при попытке несанкционированного изменения состояния пинов безопасности или для фиксации точного времени события.*/
    EXTI22_RTC_WKUP_Handler,        /*Пробуждение по RTC. Периодическое прерывание от RTC для вывода микроконтроллера из энергосберегающих режимов (Stop, Standby).*/
    FLASH_Handler,                  /*Прерывание Flash-памяти. Сообщает о завершении операции записи/стирания Flash или возникновении ошибки программирования памяти.*/
    RCC_Handler,                    /*Управление тактированием (RCC). Срабатывает при стабилизации работы генераторов (HSE, HSI, PLL) или при аварийном отключении внешнего кварца (CSS — Clock Security System).*/

    /*Внешние прерывания EXTI (Порты ввода-вывода)*/

    EXTI0_Handler,                  /*Обработка прерывания на линиях GPIO с номером 0 (PA0, PB0...).*/
    EXTI1_Handler,                  /*Обработка прерывания на линиях GPIO с номером 1 (PA1, PB1...).*/
    EXTI2_Handler,                  /*Обработка прерывания на линиях GPIO с номером 2 (PA2, PB2...).*/
    EXTI3_Handler,                  /*Обработка прерывания на линиях GPIO с номером 3 (PA3, PB3...).*/
    EXTI4_Handler,                  /*Обработка прерывания на линиях GPIO с номером 4 (PA4, PB4...).*/

    /*Контроллеры прямого доступа к памяти (DMA)*/

    DMA1_Stream0_Handler,           /*Прерывание потока 0 DMA1 (передача завершена, половина передана, ошибка).*/
    DMA1_Stream1_Handler,           /*Прерывание потока 1 DMA1.*/
    DMA1_Stream2_Handler,           /*Прерывание потока 2 DMA1.*/
    DMA1_Stream3_Handler,           /*Прерывание потока 3 DMA1.*/
    DMA1_Stream4_Handler,           /*Прерывание потока 4 DMA1.*/
    DMA1_Stream5_Handler,           /*Прерывание потока 5 DMA1.*/
    DMA1_Stream6_Handler,           /*Прерывание потока 6 DMA1.*/

    ADC_Handler,                    /*Срабатывает при завершении регулярного/инжектированного преобразования АЦП, аналоговом "watchdog" (выход сигнала за границы напряжения) или ошибке переполнения данных (OVR).*/

    /*Внешние прерывания EXTI (Порты ввода-вывода)*/

    EXTI9_5_Handler,                /*Групповое прерывание. Обрабатывает события на линиях GPIO от 5 до 9 (нужно программно опрашивать регистр EXTI->PR, чтобы понять, какая именно нога сработала).*/

    TIM1_BRK_TIM9_Handler,          /*Аварийный останов (Break) таймера 1 (для защиты силовых ключей) И прерывание базового таймера 9.*/
    TIM1_UP_TIM10_Handler,          /*Событие обновления (Update/Переполнение) таймера 1 И прерывание базового таймера 10.*/
    TIM1_TRG_COM_TIM11_Handler,     /*События триггера/коммутации таймера 1 И прерывание базового таймера 11.*/
    TIM1_CC_Handler,                /*События захвата/сравнения (Capture/Compare) каналов 1–4 таймера 1.*/
    TIM2_Handler,                   /*Полный спектр событий 32-битного таймера 2.*/
    TIM3_Handler,                   /*События таймера 3.*/
    TIM4_Handler,                   /*События таймера 4.*/
    I2C1_EV_Handler,                /*События (Events). Срабатывают при отправке адреса, приеме/передаче байта, условиях START/STOP.*/
    I2C1_ER_Handler,                /*Ошибки (Errors). Срабатывают при ошибке подтверждения (NACK), потере арбитража шины (ARLO) или ошибках шины (BERR).*/
    I2C2_EV_Handler,                /*События (Events). Срабатывают при отправке адреса, приеме/передаче байта, условиях START/STOP.*/
    I2C2_ER_Handler, /*Ошибки (Errors). Срабатывают при ошибке подтверждения (NACK), потере арбитража шины (ARLO) или ошибках шины (BERR).*/
    SPI1_Handler, /*енерируются при заполнении буфера приемника (RXNE), освобождении буфера передатчика (TXE) или системных ошибках*/
    SPI2_Handler, /*енерируются при заполнении буфера приемника (RXNE), освобождении буфера передатчика (TXE) или системных ошибках*/
    USART1_Handler, /*Прерывания приемопередатчиков USART. Сигнализируют о приеме байта (RXNE), завершении передачи (TC), освобождении регистра данных (TXE) или ошибках кадра (IDLE/PE/FE).*/
    USART2_Handler, /*Прерывания приемопередатчиков USART. Сигнализируют о приеме байта (RXNE), завершении передачи (TC), освобождении регистра данных (TXE) или ошибках кадра (IDLE/PE/FE).*/

    /*Внешние прерывания EXTI (Порты ввода-вывода)*/

    EXTI15_10_Handler,/*Групповое прерывание. Обрабатывает события на линиях GPIO от 10 до 15.*/
    EXTI17_RTC_Alarm_Handler,/*Будильник часов реального времени (RTC) через линию EXTI17.*/
    EXTI18_OTG_FS_WKUP_Handler,/*Пробуждение системы по активности на шине USB OTG FS (через линию EXTI18).*/

    /*Контроллеры прямого доступа к памяти (DMA)*/

    DMA1_Stream7_Handler, /*Прерывание потока 7 DMA1.*/

    SDIO_Handler,/*Обрабатывает события обмена данными, команды и ошибки при работе с SD-картами.*/
    TIM5_Handler,/*События таймера 5.*/
    SPI3_Handler,/*енерируются при заполнении буфера приемника (RXNE), освобождении буфера передатчика (TXE) или системных ошибках*/

    /*Контроллеры прямого доступа к памяти (DMA)*/

    DMA2_Stream0_Handler, /*Прерывание потока 0 DMA2.*/
    DMA2_Stream1_Handler, /*Прерывание потока 1 DMA2.*/
    DMA2_Stream2_Handler, /*Прерывание потока 2 DMA2.*/
    DMA2_Stream3_Handler, /*Прерывание потока 3 DMA2.*/
    DMA2_Stream4_Handler, /*Прерывание потока 4 DMA2.*/

    OTG_FS_Handler, /*Основное прерывание работы контроллера USB в режиме Full Speed (события подключения, сброса шины, приема/передачи пакетов в конечных точках Endpoint).*/

    /*Контроллеры прямого доступа к памяти (DMA)*/

    DMA2_Stream5_Handler, /*Прерывание потока 5 DMA2.*/
    DMA2_Stream6_Handler, /*Прерывание потока 6 DMA2.*/
    DMA2_Stream7_Handler, /*Прерывание потока 7 DMA2.*/

    USART6_Handler, /*Прерывания приемопередатчиков USART. Сигнализируют о приеме байта (RXNE), завершении передачи (TC), освобождении регистра данных (TXE) или ошибках кадра (IDLE/PE/FE).*/
    I2C3_EV_Handler,/*События (Events). Срабатывают при отправке адреса, приеме/передаче байта, условиях START/STOP.*/
    I2C3_ER_Handler,/*Ошибки (Errors). Срабатывают при ошибке подтверждения (NACK), потере арбитража шины (ARLO) или ошибках шины (BERR).*/
    FPU_Handler, /*Срабатывает при исключительных ситуациях во встроенном математическом сокалькуляторе FPU (переполнение, деление на ноль, потеря точности при работе с числами float).*/
    SPI4_Handler,/*енерируются при заполнении буфера приемника (RXNE), освобождении буфера передатчика (TXE) или системных ошибках*/
    SPI5_Handler,/*енерируются при заполнении буфера приемника (RXNE), освобождении буфера передатчика (TXE) или системных ошибках*/    
};


void Default_Handler(void)
{
    while(1)
    {

    }
}



