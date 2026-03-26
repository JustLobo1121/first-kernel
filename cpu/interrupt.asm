[bits 32]

[extern keyboard_handler] 

global isr32
global isr33

isr32:
    pusha
    mov al, 0x20
    out 0x20, al
    popa
    iretd

isr33:
    pusha
    call keyboard_handler
    popa
    iretd