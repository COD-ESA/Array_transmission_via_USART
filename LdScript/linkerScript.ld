/* size flash  512Kb*/
/* size RAM 128Kb */

/* start address flash 0x08000000  0x0807ffff*/
/* start address RAM 0x20000000 0x20020000 */

ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM); /* end of "RAM" Ram type memory */

/*_Min_Heap_Size = 0x200;*/ /* required amount of heap */
/*Если ты не используешь malloc и функции динамической памяти,куча не нужна.*/
    /* PROVIDE ( end = . ); */ 
_Min_Heap_Size = 0; /* required amount of heap */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
    FLASH   (rx)    : ORIGIN = 0x08000000, LENGTH = 512K 
    RAM     (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K   
}

SECTIONS
{
    .isr_vector : 
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text : 
    {
        . = ALIGN(4);
        *(.text*)          /* Потом остальные функции */
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    _sidata = LOADADDR(.data);

    .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

   . = ALIGN(4);
  .bss : {
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM

    ._user_heap_stack :
  {
    . = ALIGN(8);
    /*Символы end и _end:
    Если ты не используешь malloc и функции динамической памяти, они тебе не нужны.*/
    /* PROVIDE ( end = . ); */ 
    /* PROVIDE ( _end = . ); */
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

    /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
  /* Всё остальное — под нож, если не используешь специфику C++ */


}