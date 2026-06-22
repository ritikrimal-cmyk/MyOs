#ifndef TIMER_H
#define TIMER_H

typedef unsigned int uint32_t;

void timer_init(uint32_t frequency);
uint32_t timer_get_ticks();

#endif
