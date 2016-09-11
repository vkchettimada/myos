#ifndef _MYOS_SEM_H_
#define _MYOS_SEM_H_

#include <myos/myos.h>
#include <myos/myos_queue.h>

typedef struct {
	int value;
	void *pQueuedTasks;
} MyOsSem;

void myOsSemWait(MyOsSem *s);
void myOsSemSignal(MyOsSem *s);
int myOsSemPending(MyOsSem *s);

#endif

