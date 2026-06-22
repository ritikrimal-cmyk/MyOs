.section .text
.global context_switch

# context_switch(Registers* old, Registers* new)
context_switch:
    mov 4(%esp), %eax    # old registers pointer
    mov 8(%esp), %ecx    # new registers pointer

    # Save old context
    mov %ebx, 4(%eax)
    mov %esi, 16(%eax)
    mov %edi, 20(%eax)
    mov %ebp, 24(%eax)
    mov %esp, 28(%eax)
    pushf
    pop 36(%eax)

    # Restore new context
    mov 4(%ecx),  %ebx
    mov 16(%ecx), %esi
    mov 20(%ecx), %edi
    mov 24(%ecx), %ebp
    mov 28(%ecx), %esp
    push 36(%ecx)
    popf

    # Jump to new process entry point
    mov 32(%ecx), %eax
    jmp *%eax

.section .note.GNU-stack, "", @progbits
