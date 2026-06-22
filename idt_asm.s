.section .text

.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    cli
    pushl $0
    pushl $\num
    jmp isr_common
.endm

.macro ISR_ERRCODE num
.global isr\num
isr\num:
    cli
    pushl $\num
    jmp isr_common
.endm

.macro IRQ num irqnum
.global irq\num
irq\num:
    cli
    pushl $0
    pushl $\irqnum
    jmp irq_common
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19

IRQ 0, 32
IRQ 1, 33

.extern isr_handler
isr_common:
    pushal
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    call isr_handler
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popal
    addl $8, %esp
    iret

.extern irq_handler
irq_common:
    pushal
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    call irq_handler
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popal
    addl $8, %esp
    iret

.section .note.GNU-stack, "", @progbits
