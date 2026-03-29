[bits 32]

global isr0
[extern isr0_handler]
global isr14
[extern isr14_handler]
global isr32
[extern timer_handler]
global isr33
[extern keyboard_handler]
global isr46


isr0:
    cli
    pusha
    call isr0_handler
    jmp $

isr14:
    cli
    pusha
    call isr14_handler
    jmp $

isr32:
    pusha
    call timer_handler
    popa
    iretd

isr33:
    pusha
    call keyboard_handler
    popa
    iretd

isr46:
    pusha
    mov al, 0x20
    out 0xA0, al 
    out 0x20, al 
    popa
    iretd