/*
 * The stack starts at the top of memory, defined by __sp, and grows down.
 * Unfortunately this means we must hardcode the size of memory.
 * Here we choose 64K. Change this value to change the working memory size.
 */
__mem   = 0x80000000; /*This should be set to the beginning of the memory as specified by the device tree, for spike run: "spike --dump-dts a.out"*/
__stack = 0x01000000; /*This is the offset that the stack is from the start of memory.*/

SECTIONS
{
    . = __mem;
    .text : {
        *(.text.init) /* add the instruction from init.s*/
        *(.text) /* add the instructions compiled from hello.c*/
    }

    .data : {
        *(.sbss)
        *(COMMON)
        *(.bss)
        *(.sdata)
        *(.rodata)
        *(.data*)
        *(.got*)
    }
}
