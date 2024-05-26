## Questions
- is uboot required at all?
  - don't think so bc the SPL can be read from the SD card. SPL can do hw init
    and load kernel directly
- what hw initialization is required in start.S?
  - stack pointers init
  - bss init
  - other?
- going to use UART to display the output of the OS
  - where can i display the output
  - do i need to run a program on my laptop and connect the bbb via uart to it?
  - A: use screen terminal command
  - A: qemu will display output in running shell by default
- can we get JTAG over USB to single-step through the bare metal code?
  - may not be needed if a good dev env is set up w qemu
- is etcher needed to configure the microsd card correctly for it to be flashed
  from?
- The MLO should be in a file called "MLO" in the root of the uSD filesystem?
  (source:
  https://github.com/mvduin/bbb-asm-demo/?tab=readme-ov-file#booting-%CE%BCsdemmc)


## TODO now
- run what you have rn with qemu (realview arch)
  - worthwhile investing in developing a parallel environment in qemu (which, 
    according to 432 experience, is more forgiving than bare metal anyway)
- to demonstrate that the loaded image is indeed running, write _start, jump
  to a C file, and show distinct pattern on LEDs to verify that init code
  is running

## bootloader
### Research
source: https:/developer.arm.com/documentation/ka002218/latest/
- The bootloader is usually placed at the chips flash base address

source: ARM Cortex A8 TRM
- The Reset exception always branches to 0x00000000
  - where bootloader should begin
  - actually is that where 1st stage boot code is in ROM?

BBB SRM 6.7
- primary boot source is eMMC
- hold the Boot button to fore the board to boot from the microSD slot
  - may not need to, since the boot code checks all available devices for block
    headers (TODO: check terminology here)
- after the primary boot code in the processor ROM passes control, a secondary
stage (SPL or MLO) takes over
- SPL initializes only the required devices to continue the boot process

source: https://github.com/victronenergy/venus/wiki/bbb-boot-process-overview
- 4 stages to the BBB boot process
  - built-in ROM loads at powerup and immediately looks for a second-stage
    bootloader with a known name "MLO" on the boot device's first partition
    formatted as FAT
  - the MLO does some basic initialization and loads a third-stage bootloader,
    the full u-boot.img, from the same boot device
  - u-boot loads the zImage kernel and a DTS into
    memory and boots the kernel with some command line args
  - the kernel boots and loads the root filesystem
- by default, the first boot device is the eMMC (MMC1)
- by default, the BBB u-boot always looks for a kernel and DTS from the SD card
  (MMC0) before the eMMC (MMC1) regardless of the boot device that MLO and
  u-boot.img loaded from
- if no MLO is found on the eMMC, the ROM will try the SD card automatically
- the MLO always loads the u-boot.img from the same device it's running from

GP header
The “Destination” address field stands for both:
- target addr for the image copy (internal RAM or SDRAM)
- entry point for the image code
* in other words, the user must locate the code entry point to the target addr
  for image copy

TOC (table of contents)
- only needed in GP devices while using MMC RAW mode
- consists of a max of 2 items
  - second TOC item must be fuilled by FFh
- each TOC item contains information required by the ROM Code to find a valid
  image in RAW mode
- 

## Useful commands
- `ojbdump` -- disassembler but it can also display various other information about object ﬁles
  - `objdump -t`
    - display symbol table
  - `objdump -p`
    - print ELF program headers
- `nm` -- lists symbols from object ﬁles
- `readelf` -- displays information from ELF-format object ﬁle
- `size` displays the sections of an object or archive, and their sizes
- `xxd` creates a hexdump of a given file


## General Terminology
Code shadowing
- copying code from a non-directly addressable area (e.g. memory device) to
  a location where code can be executed (typically RAM)

CRC
- Cyclic Redundancy Check
- error-detecting code commonly used in digital networks and storage devices
  to detect accidental changes to digital data

eMMC
- embedded Multi Media Card

DMIPS
- Dhrystone Millions of Instructions Per Second
- Dhrystone is an integer-only benchmark
- Dhrystones per second (the number of iterations of the main code loop)

DPLL
- Digital Phase Locked Loop
- boosts clock frequency

DTS
- Device Tree Structure

FPB
- breakpoint unit

GPIO
- General Purpose I/O
- uncommitted digital signal pin on an IC or electric circuit (MCUs / MPUs)

GPMC
- General Purpose Memory Controller

MCU 
- Microcontroller Unit

MLO
- Memory Loader
- runs from internal SRAM

MMC
- Multi Media Card

MMU
- Memory Management Unit

MPU
- Memory Protection Unit
- trimmed down version of MMU

Northbridge
- aka host bridge or memory controller hub
- a microchip that comprises the core logic chipset architecture on
  motherboards to handle high-performance tasks
- connected to a CPU via the front-side bus (FSB)
  - back-side bus connects the CPU to the cache
- usually used in conjunction with a slower southbridge to manage communication
  between the CPU and other parts of the motherboard

NVIC
- Nested Vectored Interrupt Controller
- programmable priority levels

ONFI
- Open NAND Flash Interface

PLL
- Phase Locked Loop

PMIC
- Power Management Integrated Circuit

RCA
- Relative Card Address

SATA
- Serial AT Attachment
- computer bus interface that connects host bus adapters to mass storage
  devices such as HDDs, optical drives, SSDs

Sector
- logical unit of 512 bytes (on AM335x)

SPI
- Serial Peripheral Interface

SPL
- Secondary Program Loader
- sometimes referred to as MLO (MMC Loader)

SRM
- System Reference Manual

UART
- Universal Asynchronous Receiver/Transmitter
- serial
- data format and transmission speeds are configurable
- sends bits one-by-one, from the least significant to the most significant
- communication may be in 3 modes
  - simplex
    - one direction only, with no provision for receiver to respond
  - full duplex
    - both devices send and receive at the same time
  - half duplex
    - devices take turns transmitting and receiving

Watchdog timer
- timer used to detect and recover form hardware malfunctions
- during normal execution the watchdog timer is restarted to prevent hardware
  from elapsing or timing out
- if, due to a hw fault or program error, the computer fails to reset the timer, 
  the timer will elapse and generate a timeout signal
- the timeout signal often invokes a reboot

XIP
- eXecutable In Place


## ARM programming terminology
APSR
- Application Program Status Register (APSR)
- this is an application-level alias for the CPSR
-  31  30  29  28  27    24            20        16                           0
  N | Z | C | V | Q | RAZ/ | Reserved,  | GE[3:0] | Reserved, UNK / SBZP      |
                    | SBZP | UNK / SBZP |         |                           |
- this view doesn't tell me anything useful about bits 0 - 27
source:
https://developer.arm.com/documentation/ddi0406/c/Application-Level-Architecture/Application-Level-Programmers--Model/The-Application-Program-Status-Register--APSR-?lang=en

CPSR:
- Current Program Status Register
- holds processor status and control information:
  - the APSR
  - current instruction set state
  - execution state bits for the Thumb If-Then instruction
  - current endianness
  - **current processor mode**
  - **interrupt and async abort disable bits**

SPSRs
- Saved Program Status Registers
- the purpose is to record the pre-exception value of the CPSR
- **on taking the exception, the CPSR is copied to the SPSR of the mode to
  which the exception is taken**

Format of CPSR and SPSR bit assignments:
https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/Program-Status-Registers--PSRs-?lang=en#CIHJBHJA


## ARM processor modes
For details and usage information, see source:
https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-processor-modes?lang=en#CIHGHDGI

Processor modes   Encoding Privilege level  Implemented
User         usr  10000    PL0              Always
FIQ          fiq  10001    PL1              Always
IRQ          irq  10010    PL1              Always
Supervisor   svc  10011    PL1              Always
Monitor      mon  10110    PL1              With security 
                                            extensions
Abort        abt  10111    PL1              Always
Hyp          hyp  11010    PL2              With virtualization
                                            extensions
Undefined    und  11011    PL1              Always
System       sys  11111    PL1              Always


to change modes:
msr cpsr_<fields>
where <fields> is a sequence of one or more of the following
- c
  - mask<0> = '1' to enable writing of bits<7:0> of the destination PSR
- x
  - mask<1> = '1' to enable writing of bits<15:8> of the destination PSR
- s
  - mask<2> = '1' to enable writing of bits<23:16> of the destination PSR
- f
  - mask<3> = '1' to enable writing of bits<31:24> of the destination PSR.
source: https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/System-Instructions/Alphabetical-list-of-instructions/MSR--immediate-?lang=en

important ARM assembly constants
https://developer.arm.com/documentation/dui0056/d/writing-code-for-rom/loading-the-rom-image-at-address-0/sample-code

FIQ
- reserved for a single, high-priority interrupt source that requires a guaranteed fast response time

IRQ
- used for all other interrupts in the system
- As FIQ is the last entry in the vector table, the FIQ handler can be placed
  directly at the vector location and run sequentially from that address. This
  avoids a branch instruction and any associated delay, speeding up FIQ
  response time
  - when I_Bit is set, IRQ is disabled
- A further key difference between IRQ and FIQ is that the FIQ handler is not
  expected to generate any other exceptions. FIQ is therefore reserved for
  special system-specific devices which have all their memory mapped and no
  need to make SVC calls to access kernel functions (so FIQ can be used only by
  code which does not need to use the kernel API).
  - when F_Bit is set, FRQ is disabled

