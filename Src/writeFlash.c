/*writeFlash.c*/

/*
Для чтения и записи во флеш нужно сначала:
1 разблокировать флеш
    записывать данные можно размерами byte, half word, word and double word
2 существует два режима программирования памяти у F4:
    Polling mode using HAL_FLASH_Program() function - режим опроса
    Interrupt mode using HAL_FLASH_Program_IT() function - режим с прерываниями
*/

#include "main.h"

/*Unlock/Lock flash */
static inline int lockUnlockFlash(void)
{
    //проверяю заблокирован ли FLASH->CR если да то разблок-ю
    if(FLASH -> CR & FLASH_CR_LOCK) {
        __disable_irq();
        FLASH -> KEYR = 0x45670123;
        FLASH -> KEYR = 0xCDEF89AB;
        __enable_irq();
    }
    //проверяю успешно ли прошла разбл-ка        
    return FLASH -> CR & FLASH_CR_LOCK? 1 : 0;
}

static inline void writeFlash(uint32_t addr, uint8_t data)
{
    *(__IO uint8_t*)addr = data;
}

/*очистка N сектора флеш*/
static inline void eraseFlash(uint32_t sector)
{
    while(lockUnlockFlash());

    // 1. ждем освобождения
    while (FLASH->SR & FLASH_SR_BSY); // Ждем, пока флеш занят

    // 2. Сбрасываем старые ошибки во FLASH_SR (rc_w1)
    FLASH->SR = FLASH_SR_PGSERR | FLASH_SR_PGAERR | 
                FLASH_SR_PGPERR | FLASH_SR_WRPERR;
    
    uint32_t cr_cnfg = FLASH -> CR;
    cr_cnfg &= ~(FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_MER | FLASH_CR_SNB | FLASH_CR_PSIZE);
    cr_cnfg |= sector | FLASH_CR_SER | FLASH_CR_PSIZE_1;
    FLASH -> CR = cr_cnfg;
    FLASH -> CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY); // Ждем, пока флеш занят
    FLASH -> CR &= ~FLASH_CR_SER;
}



void readORwrite_of_Flash(const _Bool status, const uint32_t start_address, const uint32_t quantity_byte, uint32_t sector)
{
    uint32_t end_address = start_address + quantity_byte; 

    //разблокировка флеш памяти
    while(lockUnlockFlash());

    //стираю сектор флеш памяти
    eraseFlash(sector);

    //запись в флеш
    if(status){
        __disable_irq();

        uint32_t cr_cnfg = FLASH -> CR;
        cr_cnfg &= ~(FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_MER | FLASH_CR_SNB | FLASH_CR_PSIZE);

        /*вктивирую режим записи, ширина шины записи х32*/
        cr_cnfg |= sector | FLASH_CR_PG;
        FLASH -> CR = cr_cnfg;

         __enable_irq();

        for (uint32_t address = start_address; address < end_address; address++){
            while (FLASH->SR & FLASH_SR_BSY); // Ждем, пока флеш занят
            writeFlash(address, 0xAA); 
            while (FLASH->SR & FLASH_SR_BSY); // Ждем, пока флеш занят
            
            /*эТО УСЛОВИЕ ОТРАБОТАЕТ только в том случае, если разрешены прерывания по завершении операции (EOPIE = 1 в регистре FLASH_CR).*/
            if(FLASH -> SR & FLASH_SR_EOP) {
                FLASH -> SR = FLASH_SR_EOP;    
            }
        }

       /*отключаю режим программирования флеш памяти*/
        FLASH -> CR &= ~FLASH_CR_PG;

        /*блокирую регистр FLASH->CR*/
        FLASH -> CR |= FLASH_CR_LOCK;
        
    //чтение из флеш        
    }else{
        /*тут будет уод чтения из флеш*/
    }
}