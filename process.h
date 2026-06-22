#ifndef PROCESS_H
#define PROCESS_H
#include "idt.h"

typedef unsigned int uint32_t;

#define MAX_PROCESSES  16
#define STACK_SIZE     4096

enum ProcessState {
    PROCESS_FREE    = 0,
    PROCESS_READY   = 1,
    PROCESS_RUNNING = 2,
    PROCESS_BLOCKED = 3
};

struct Process {
    int        pid;
    char       name[32];
    ProcessState state;
    Registers  regs;
    uint32_t   stack[STACK_SIZE / 4];
};

void process_init();
int  process_create(const char* name, void (*entry)());
void process_exit();
void scheduler_tick();
int  process_count();

#endif
