MEMORY
{
    sram : ORIGIN = 0x402F0400, LENGTH = 0xFBFF /* am335x TRM 26.1.4.2 */
}

SECTIONS
{
  .text: {
    . = ALIGN(4);
    *(.o*)
    *(.text*) 
  } > sram

  .data : { 
    . = ALIGN(4);
    *(.data) 
  } > sram

  .bss : { 
    _bss_start = .;
    . = ALIGN(4);
    *(.bss) 
    _bss_end = .;
  } > sram

  .stack :
  {
    _stack_limit = .;
    . = ALIGN(4);
    *(.stack*)
    . = . + 0x2000; /* 8KB of stack for 4 modes */
    _stack_top = .;
  } > internal_ram

  . = ALIGN(4);
}

ENTRY(_start)

