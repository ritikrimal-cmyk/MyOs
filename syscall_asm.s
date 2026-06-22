.section .text

.global syscall_stub
syscall_stub:
    cli
    pushl $0
    pushl $0x80
    pushal
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    call syscall_handler
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popal
    addl $8, %esp
    iret

.section .note.GNU-stack, "", @progbits
