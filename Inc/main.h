/*main.h*/

#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f411xe.h"
#include "stdio.h"

#define PROGRAMM_FLASH 1
#define ERASE_FLASH 0
#define Flash_START_ADDRESS 0x08008000UL
#define Flash_QUANTITY_BYTE 16384UL

#define FLASH_SECTOR_0   (0UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_1   (1UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_2   (2UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_3   (3UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_4   (4UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_5   (5UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_6   (6UL << FLASH_CR_SNB_Pos)
#define FLASH_SECTOR_7   (7UL << FLASH_CR_SNB_Pos)


void Default_Handler(void);
void InitFlash(void);
void config_clock_and_output_PC(void);
void toglle_pc13(void);
void toggle_pc13_mod(void);
void config_mco1(void);
void readORwrite_of_Flash(const _Bool status, const uint32_t start_address, const uint32_t quantity_byte, uint32_t sector);


#endif /* __MAIN_H */