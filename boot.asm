MBALIGN equ 1<<0
MEMINFO equ 1<<0
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC+FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

section .bss
align 4
stack_bottom:
resb 16384
stack_top:

section .text
global _start:function (_start.end - _start)

_start:
mov esp,stack_top

extern kernel_main
call kernel_main
cli
hlt
.end:

%macro interrupt 1
global irq_%1
extern %1
irq_%1:
    pusha
    push ds
    push es
    push fs
    push gs
    push eax
    call %1
    mov al,0x20
    out 0x20,al
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
%endmacro


interrupt defaulte
interrupt error
interrupt hdd