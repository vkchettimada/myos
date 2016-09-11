/* myos_queue.h */
#ifndef _MYOS_QUEUE_H_
#define _MYOS_QUEUE_H_

#include <myos/myos.h>

typedef struct
{
	void *pQueue;
	int data_size;
	int queue_size;
	int first;
	int last;

	void *s;
} MyOsQueue;

void myOsQueueSem(MyOsQueue *q, void *s);
int myOsQueueEmpty(MyOsQueue *q);
int myOsQueueFull(MyOsQueue *q);
int myOsQueueAdd(MyOsQueue *q, void *data);
int myOsQueueRemove(MyOsQueue *q, void *data);

#endif

