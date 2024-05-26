.global _start

_start:
   ; Initialize stack for all modes
   ; https://developer.arm.com/documentation/dui0471/m/embedded-software-development/stack-pointer-initialization
   
   ; Each exception mode (the privileged modes, other than System, ie. SVC,
   ; UND, ABT, IRQ, FIQ) which are entered from User mode when an exception
   ; occurs, such as an undefined instruction, or a hardware interrupt) has its
   ; own R13 (SP) and R14 (LR) registers. (FIQ also has its own R8-R12).

   ; enter each mode in turn and set up the stack pointer
    ldr     r0, _stack_top
    msr     cpsr_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit    ; Interrupts disabled -- if I_Bit is 1, IRQ disabled
    mov     sp, r0
    sub     r0, r0, #0x800                         ; move sp down 2KB
    msr     cpsr_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit    ; Interrupts disabled
    mov     sp, r0
    sub     r0, r0, #0x800
    msr     cpsr_c, #Mode_SVC:OR:I_Bit:OR:F_Bit    ; Interrupts disabled
                                                   ; Kernel should run in SVC mode
                                                   ; System starts in SVC on reset in ARMv7-A
    mov     sp, r0
    /* TODO: initialize BSS */
    bl      _main                                  ; TODO: does this work?

