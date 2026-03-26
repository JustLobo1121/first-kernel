C_SOURCES = kernel/kernel.c drivers/ports.c cpu/idt.c cpu/isr.c drivers/keyboard.c libc/string.c drivers/speaker.c
OBJ = kernel/kernel_entry.o kernel/kernel.o drivers/ports.o cpu/idt.o cpu/isr.o cpu/interrupt.o drivers/keyboard.o libc/string.o drivers/speaker.o

run: os_image.bin
	qemu-system-x86_64 -drive format=raw,file=os_image.bin

os_image.bin: boot/boot.bin kernel/kernel.bin
	cat boot/boot.bin kernel/kernel.bin > os_image.bin
	dd if=/dev/zero bs=512 count=40 >> os_image.bin

kernel/kernel.bin: $(OBJ)
	ld -m elf_i386 -Ttext 0x1000 --oformat binary $^ -o $@

%.o: %.c
	gcc -ffreestanding -m32 -fno-pie -c $< -o $@

%.o: %.asm
	nasm -f elf $< -o $@

boot/boot.bin: boot/boot.asm
	nasm -f bin $< -o $@

clean:
	rm -f boot/*.bin kernel/*.bin kernel/*.o drivers/*.o cpu/*.o *.bin libc/*.o