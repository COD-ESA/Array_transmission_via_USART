/* InitFlash.c */

#include "main.h"

void InitFlash(void)
{
    /*1*/
    FLASH -> ACR = (
        FLASH_ACR_PRFTEN    |   //Позволяет флешу заранее подгружать следующие слова
        FLASH_ACR_ICEN      |   //Включает кеш инструкций (чтение команд из Flash)
        FLASH_ACR_DCEN      |   //Включает кеширование данных (операции с переменными, массивами в Flash)
        FLASH_ACR_LATENCY_4WS   //Устанавливаем 4 wait states (для 100 MHz)
    );

    /*2*/
    // FLASH -> ACR |= FLASH_ACR_LATENCY_3WS; // Устанавливаем 3 wait states (для 96 MHz)
    (void)FLASH -> ACR; // Чтение регистра для синхронизации

    // 3. Проверим, применилось ли значение (опционально)
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_4WS);
    // while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_3WS);
}