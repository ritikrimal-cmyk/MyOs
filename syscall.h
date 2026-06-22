#ifndef SYSCALL_H
#define SYSCALL_H

typedef unsigned int uint32_t;

// Syscall numbers
#define SYS_PRINT   0
#define SYS_MALLOC  1
#define SYS_FREE    2
#define SYS_EXIT    3
#define SYS_CLEAR   4

void syscall_init();

#endif
