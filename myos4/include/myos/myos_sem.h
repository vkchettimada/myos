#ifndef _MYOS_SEM_H_
#define _MYOS_SEM_H_

#include <myos/myos.h>

typedef struct {
	unsigned char value;
	int task;
} MyOsSem;

void myos_wait(MyOsSem *s);
void myos_signal(MyOsSem *s);

#endif

