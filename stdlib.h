#ifndef STDLIB_H
#define STDLIB_H

typedef unsigned int size_t;

int   atoi(const char* str);
void  itoa(int val, char* buf, int base);
void* malloc(size_t size);
void  free(void* ptr);

#endif
