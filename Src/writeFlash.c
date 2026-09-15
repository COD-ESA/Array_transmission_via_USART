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

/* Unlock/Lock flash */
static inline uint32_t lockUnlockFlash(void)
{
    //проверяю заблокирован ли FLASH->CR если да то разблок-ю
    if(FLASH -> CR & FLASH_CR_LOCK) {
        FLASH -> KEYR = 0x45670123;
        FLASH -> KEYR = 0xCDEF89AB;
    }
    //проверяю успешно ли прошла разбл-ка        
    return (FLASH -> CR & FLASH_CR_LOCK);
}

/*Функция записывает data по адресу addr*/
static inline void writeFlash8(uint32_t addr, uint8_t data)
{
    *(__IO uint8_t*)addr = data;
}
static inline void writeFlash32(uint32_t addr, uint32_t data)
{
    *(__IO uint32_t*)addr = data;
}

// Универсальный макрос: автоматически выбирает нужную функцию по типу 'data'
#define writeFlash(addr, data) _Generic((data), \
    uint8_t:  writeFlash8,                      \
    uint32_t: writeFlash32                      \
)(addr, data)


/*
Функция проверяет в цикле условие о разблокировке флеш и возвращает результат работы цикла:
флеш заблокирована = 1,  флеш ращблокирована = 0.
*/
static inline uint8_t Flash_answer_BSY(void)
{
    uint8_t flash_bsy = 1;
    uint8_t flash_success_unlock_bsy = 0;
    uint32_t timeout = 100000U;
    while ((FLASH->SR & FLASH_SR_BSY) && timeout--); // Ждем, пока флеш занят
    if(timeout){
        return flash_success_unlock_bsy;
    }
    return flash_bsy;
}

/*очистка N сектора флеш*/
static inline void eraseFlash(uint32_t sector)
{   
    // 1. жду разблокировки флеш
    if(lockUnlockFlash()){
        return;
    }

    // 2. ждем освобождения
    if(Flash_answer_BSY()){
        return;
    }

    // 3. Сбрасываем старые ошибки во FLASH_SR (rc_w1)
    FLASH->SR = FLASH_SR_PGSERR |
                FLASH_SR_PGAERR | 
                FLASH_SR_PGPERR |
                FLASH_SR_WRPERR;
    
    uint32_t cr_cnfg = FLASH -> CR;

    // 4. Сбрасываю настроечные биты FLASH -> CR
    cr_cnfg &= ~(
                FLASH_CR_PG |
                FLASH_CR_SER |
                FLASH_CR_MER |
                FLASH_CR_SNB |
                FLASH_CR_PSIZE);
    
    // 5. Выбираю сектор для стирания и размер стирания блоками по 32 бит                
    cr_cnfg |=  sector |
                FLASH_CR_SER |
                FLASH_CR_PSIZE_1;

    // 6. Записываю настройки в регистр и запускаю стирание сектора памяти            
    FLASH -> CR = cr_cnfg;
    FLASH -> CR |= FLASH_CR_STRT;
    
    // 7. Ждем, когда флеш освободится
    if(Flash_answer_BSY()){
        return;
    }

    // 8. Выключаю режим стирания сектора памяти
    FLASH -> CR &= ~FLASH_CR_SER;
}

/*
status                          Если status = 0 то настраиваю регистры под чтение флеш, если status = 1 то настраиваю регистры под запись во флеш.
start_address                   Адрес флеш с которого нужно начать чтение или запись данных. 
quantity_byte                   Какое количество нужно прочитать или записать во флеш
sector                          Какой сектор нужно очистить
uint32_t (*func)(uint32_t)      Функция возвращает то число которое получило как аргумент.
*/
void readORwrite_of_Flash(const _Bool status, const uint32_t start_address, const uint32_t quantity_byte, uint32_t sector, uint32_t (*func)(uint32_t))

{   
    uint32_t end_address = start_address + quantity_byte; 

    // 1. жду разблокировки флеш
    if(lockUnlockFlash()){
        return;
    }

    //стираю сектор флеш памяти
    eraseFlash(sector);

    //запись в флеш если status=1
    if(status){
        __disable_irq();

        uint32_t cr_cnfg = FLASH -> CR;
        cr_cnfg &= ~(
                    FLASH_CR_PG |
                    FLASH_CR_SER |
                    FLASH_CR_MER |
                    FLASH_CR_SNB |
                    FLASH_CR_PSIZE);

        /*режим записи - ширина шины записи х8*/
        // cr_cnfg |= FLASH_CR_PG;

        /*режим записи - ширина шины записи х32*/
        cr_cnfg |= FLASH_CR_PG | FLASH_CR_PSIZE_1;
        FLASH -> CR = cr_cnfg;

         __enable_irq();
        
        /* Определяем инкремент адреса в зависимости от выбранного PSIZE */
        uint32_t inc_addr = (FLASH->CR & FLASH_CR_PSIZE_1) ? 4 : 1;
        
        for (uint32_t index = 1, address = start_address; address < end_address; address += inc_addr, index++){

            // Ждем, пока флеш занят
            if(Flash_answer_BSY()){
                break;
            }

            // Вычисляем порядковый номер: 0, 1, 2, 3...
            uint32_t data = func(index);
            // data = 0xAC;
            writeFlash(address, data); 
            // Ждем, пока флеш занят
            if(Flash_answer_BSY()){
                break;
            }
            
            /*эТО УСЛОВИЕ ОТРАБОТАЕТ только в том случае, если разрешены прерывания по завершении операции (EOPIE = 1 в регистре FLASH_CR).*/
            if(FLASH -> SR & FLASH_SR_EOP) {
                FLASH -> SR = FLASH_SR_EOP;    
            }
        }

       /*отключаю режим программирования флеш памяти*/
        FLASH -> CR &= ~FLASH_CR_PG;

        /*блокирую регистр FLASH->CR*/
        FLASH -> CR |= FLASH_CR_LOCK;
        
    //чтение из флеш если status=0
    }else{
        /*тут будет код чтения из флеш*/
    }
}

uint32_t readFlashMemory(const __IO uint32_t *start_address)
{
    return  *start_address;
}