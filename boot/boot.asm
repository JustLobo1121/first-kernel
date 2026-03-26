[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000
start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov [BOOT_DRIVE], dl 
    mov bp, 0x9000
    mov sp, bp
    call load_kernel      
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
    mov al, 40
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