PREFIX= arm-none-eabi-

CC=$(PREFIX)gcc
AS=$(PREFIX)as
LD=$(PREFIX)ld

# https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
CFLAGS= -g -mcpu=cortex-a8 -marm -static -ffreestanding -nostdlib -nostartfiles\
  -mfpu=neon -mfloat-abi=hard -mlong-calls
ASMFLAGS= -g -mcpu=cortex-a8

MLO: gen_mlo
	./gen_mlo foobar MLO

gen_mlo: gen_mlo.c
	gcc -o $@ $<

start.o: start.S
	$(AS) -o $@ -c $(ASMFLAGS) $<

PHONY: clean
clean:
	rm MLO gen_mlo
