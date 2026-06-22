#include "process.h"
#include "vga.h"
#include "memory.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static Process processes[MAX_PROCESSES];
static int current_pid = 0;
static int num_processes = 0;

static void str_copy(char* dst, const char* src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++)
        processes[i].state = PROCESS_FREE;

    // Create idle process (pid 0)
    processes[0].pid   = 0;
    processes[0].state = PROCESS_RUNNING;
    str_copy(processes[0].name, "idle", 32);
    num_processes = 1;
    current_pid   = 0;
}

int process_create(const char* name, void (*entry)()) {
    // Find free slot
    int slot = -1;
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_FREE) {
            slot = i;
            break;
        }
    }
    if (slot == -1) return -1;

    Process* p = &processes[slot];
    p->pid   = slot;
    p->state = PROCESS_READY;
    str_copy(p->name, name, 32);

    // Set up stack — entry point at top
    uint32_t stack_top = (uint32_t)&p->stack[STACK_SIZE / 4];
    p->regs.esp    = stack_top;
    p->regs.eip    = (uint32_t)entry;
    p->regs.eflags = 0x202; // interrupts enabled

    num_processes++;
    return slot;
}

void process_exit() {
    processes[current_pid].state = PROCESS_FREE;
    num_processes--;
    scheduler_tick(); // switch immediately
}

// Simple round-robin scheduler
void scheduler_tick() {
    int next = current_pid;

    for (int i = 1; i <= MAX_PROCESSES; i++) {
        int candidate = (current_pid + i) % MAX_PROCESSES;
        if (processes[candidate].state == PROCESS_READY ||
            processes[candidate].state == PROCESS_RUNNING) {
            next = candidate;
            break;
        }
    }

    if (next == current_pid) return; // no switch needed

    processes[current_pid].state = PROCESS_READY;
    current_pid = next;
    processes[current_pid].state = PROCESS_RUNNING;
}

int process_count() {
    return num_processes;
}
