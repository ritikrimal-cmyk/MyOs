#ifndef STRING_H
#define STRING_H

typedef unsigned int size_t;

int    strlen(const char* str);
int    strcmp(const char* a, const char* b);
int    strncmp(const char* a, const char* b, size_t n);
char*  strcpy(char* dst, const char* src);
char*  strncpy(char* dst, const char* src, size_t n);
char*  strcat(char* dst, const char* src);
char*  strchr(const char* str, int c);
void*  memset(void* dst, int val, size_t n);
void*  memcpy(void* dst, const void* src, size_t n);
int    memcmp(const void* a, const void* b, size_t n);

#endif
