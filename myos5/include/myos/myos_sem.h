#ifndef _MYOS_SEM_H_
#define _MYOS_SEM_H_

#include <myos/myos.h>
#include <myos/myos_queue.h>

typedef struct {
	int value;
	void *pQueuedTasks;
} MyOsSem;

void myos_wait(MyOsSem *s);
void myos_signal(MyOsSem *s);

#endif

