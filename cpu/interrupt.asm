[bits 32]

[extern timer_handler]
[extern keyboard_handler]

global isr32
global isr33

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