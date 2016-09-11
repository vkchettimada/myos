/* stdlib.h */
#ifndef _STDLIB_H_
#define _STDLIB_H_

void itoa (char *buf, int base, int d);
int atoi(char *s);

void *malloc(int size);
void free(void *p);

#endif
