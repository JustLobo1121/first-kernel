# Simple OS Kernel

This is a basic x86 operating system kernel written in C and Assembly.

## Initial State

Upon booting, the OS performs the following initialization sequence:

1. **Boot Process**:
   - The boot sector (`boot/boot.asm`) loads the kernel from disk into memory at address 0x1000.
   - Sets up a Global Descriptor Table (GDT) for protected mode.
   - Switches the CPU from 16-bit real mode to 32-bit protected mode.
   - Jumps to the kernel entry point.

2. **Kernel Initialization**:
   - The kernel entry (`kernel/kernel_entry.asm`) calls the main kernel function.
   - The main function (`kernel/kernel.c`) clears the screen and displays a black background.
   - Installs Interrupt Service Routines (ISRs) for hardware interrupts.
   - Remaps the Programmable Interrupt Controller (PIC) to handle interrupts starting from vector 32.
   - Sets up IDT gates for timer (ISR 32) and keyboard (ISR 33) interrupts.
   - Enables interrupts with `sti`.

3. **Initial Display**:
   - The screen is cleared to show a blank display.
   - A command prompt "OS> " is printed, indicating the OS is ready for user input.

At this point, the OS is in its initial state: a simple command-line interface waiting for keyboard input. The user can type commands and press Enter to execute them. Available commands include help, clear, matrix, beep, and sleep.

## Building and Running

Use the provided Makefile to build the OS image:
- `make run` to build `os_image.bin` and run in QEMU emulator
- `make clean` to clean build artifacts

## Dependencies

To build and run this OS in WSL (Windows Subsystem for Linux), install the following packages:

```bash
sudo apt update
sudo apt install nasm gcc binutils qemu-system-x86
```

- **nasm**: Assembler for the assembly (.asm) files
- **gcc**: C compiler with 32-bit support for compiling the kernel code
- **binutils**: Provides the linker (ld) for linking object files
- **qemu-system-x86**: x86 emulator for running the OS image