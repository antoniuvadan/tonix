SECTIONS
{
  . = 0x00000000;
  .text: { *(.text*) } /* TODO: add *.o when other object files should get linked */
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = ALIGN(4);
}

ENTRY(__start)

