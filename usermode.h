#ifndef USERMODE_H
#define USERMODE_H

typedef unsigned int uint32_t;

void usermode_init();
void usermode_jump(uint32_t entry, uint32_t user_stack);

#endif
