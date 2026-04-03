[bits 16]
[org 0x7c00]

jmp short start
nop
OEMLabel            db "MY_OS   "
BytesPerSector      dw 512
SectorsPerCluster   db 1
ReservedForBoot     dw 50
NumberOfFats        db 2
RootDirEntries      dw 512
LogicalSectors      dw 2880        
MediumByte          db 0xF8        
SectorsPerFat       dw 9
SectorsPerTrack     dw 18
Sides               dw 2
HiddenSectors       dd 0
LargeSectors        dd 0
DriveNo             dw 0
Signature           db 41          
VolumeID            dd 0x00000000
VolumeLabel         db "MY_DISC    "
FileSystem          db "FAT16   "

KERNEL_OFFSET equ 0x1000

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov [BOOT_DRIVE], dl
    mov bp, 0x9000
    mov sp, bp
    call load_kernel
    mov ah, 0x00
    mov al, 0x13
    int 0x10
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp dword CODE_SEG:init_pm 
[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov ah, 0x02
    mov al, 45
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc error_disc
    ret
error_disc:
    jmp $
BOOT_DRIVE db 0
; gdt and protected mode
gdt_start:
    dd 0x0 
    dd 0x0

gdt_code: 
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call KERNEL_OFFSET
    jmp $
times 510-($-$$) db 0
dw 0xaa55