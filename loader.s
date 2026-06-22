.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss
    .align 16
    kernel_stack_bottom:
        .space 16384
    kernel_stack_top:

.section .text
.extern kernelMain
.global loader
loader:
    mov $kernel_stack_top, %esp
    push %eax
    push %ebx
    call kernelMain
_stop:
    cli
    hlt
    jmp _stop

.section .note.GNU-stack, "", @progbits
