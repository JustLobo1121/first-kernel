# Simple OS Kernel - "MyOS"

This is a 32-bit x86 operating system kernel written in C and Assembly from scratch. It features a modular architecture, custom hardware drivers, and a fully functional FAT16 File System.

## Features & Architecture

* **Custom Bootloader:** Switches the CPU from 16-bit Real Mode to 32-bit Protected Mode and loads the kernel.
* **Interrupt Handling (IDT/ISR):** Captures hardware interrupts and CPU exceptions (Kernel Panics, Page Faults).
* **Memory Management:** * Physical Memory Manager (PMM) using a bitmap to allocate 4KB frames.
* Dynamic Memory Allocation (Heap) with `kmalloc` and `kfree`.
* **Hardware Drivers:**
* **VGA Graphics:** Supports both Text Mode (`0xB8000`) and VGA Graphic Mode 13h (`0xA0000`) with a custom 8x8 Bitmap Font.
* **Keyboard (IRQ 1):** Interactive shell with dynamic buffer, Backspace support, and `TAB` Autocompletion.
* **Disk (ATA PIO):** Raw sector reading and writing.
* **PCI Bus:** Device scanning and identification.
* **RTC & Timer:** Real-Time Clock and PIT (Programmable Interval Timer) tracking.
* **FAT16 File System:** * Reads the BIOS Parameter Block (BPB).
* Parses the Root Directory.
* Traverses the File Allocation Table (FAT) to read/write fragmented files across multiple clusters.
* Supports creating directories and tracking file metadata (size, attributes).
* **Modular Shell:** Command interpreter using an array of function pointers for easy scalability.

## Available Commands

* `help` - Shows available commands.
* `ls` - Lists files and directories in the FAT16 volume.
* `cat [file]` - Reads and prints a file following its FAT cluster chain.
* `write [name] [ext] [content]` - Allocates free clusters and saves a new file to the disk.
* `mkdir [name]` - Creates a new directory in the file system.
* `clear`, `color` - Screen formatting.
* `time`, `uptime`, `cpuinfo`, `lspci` - Hardware status.
* `alloc`, `read`, `math`, `beep` - System utilities and tests.

## Building and Running

Dependencies: `nasm`, `gcc` (32-bit cross-compiler), `binutils`, `qemu-system-x86`.

```bash
make clean
make run
```
