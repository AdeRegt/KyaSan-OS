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

stack2_bottom:
resb 26384
stack2_top:

section .text
global _start:function (_end - _start)

_start:

mov esp,stack_top
mov ebp,stack_bottom
extern kernel_main
call kernel_main
cli
hlt
_end:

%macro interrupt 1
global irq_%1
extern %1
irq_%1:
    pushad
    call %1
    mov al,0x20
    out 0x20,al
    popad
    iret
%endmacro


interrupt defaulte
interrupt error
interrupt hdd
