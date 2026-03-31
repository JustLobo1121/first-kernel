C_SOURCES = kernel/kernel.c drivers/ports.c cpu/idt.c cpu/isr.c drivers/keyboard.c libc/string.c libc/mem.c drivers/speaker.c drivers/timer.c drivers/rtc.c cpu/paging.c drivers/disk.c drivers/pmm.c drivers/pci.c drivers/vga.c fs/fat16.c
OBJ = kernel/kernel_entry.o kernel/kernel.o drivers/ports.o cpu/idt.o cpu/isr.o cpu/interrupt.o drivers/keyboard.o libc/string.o libc/mem.o drivers/speaker.o drivers/timer.o drivers/rtc.o cpu/paging.o drivers/disk.o drivers/pmm.o drivers/pci.o drivers/vga.o fs/fat16.o

run: os_image.bin
	qemu-system-x86_64 -drive file=os_image.bin,format=raw,index=0,media=disk -audiodev pa,id=speaker -machine pcspk-audiodev=speaker

os_image.bin: boot/boot.bin kernel/kernel.bin
	cat boot/boot.bin kernel/kernel.bin > os_image.bin
	dd if=/dev/zero bs=512 count=200 >> os_image.bin

kernel/kernel.bin: $(OBJ)
	ld -m elf_i386 -T linker.ld -nostdlib --oformat binary $^ -o $@

%.o: %.c
	gcc -ffreestanding -m32 -fno-pie -I include -c $< -o $@

%.o: %.asm
	nasm -f elf $< -o $@

boot/boot.bin: boot/boot.asm
	nasm -f bin $< -o $@

clean:
	rm -f boot/*.bin kernel/*.bin kernel/*.o drivers/*.o cpu/*.o *.bin libc/*.o fs/*.o