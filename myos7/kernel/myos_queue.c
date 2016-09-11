/* myos_queue.c */
#include <string.h>
#include "myos/myos_queue.h"
#include "myos/myos_sem.h"

void myOsQueueSem(MyOsQueue *q, void *s)
{
	q->s=s;
}

int myOsQueueEmpty(MyOsQueue *q)
{
	return(q->first==-1);
}

int myOsQueueFull(MyOsQueue *q)
{
	return( ((q->last+1)%q->queue_size) == q->first );
}

int myOsQueueAdd(MyOsQueue *q, void *data)
{
	char *p;
	
	if (myOsQueueFull(q))
		return(-1);
	if (q->first==-1)
		q->first=0;
	q->last = (q->last+1) % q->queue_size;
	p=(char*)q->pQueue + (q->last*q->data_size);
	memcpy(p,data,q->data_size);

	if (q->s)
		myOsSemSignal(q->s);
	
	return(0);
}

int myOsQueueRemove(MyOsQueue *q, void *data)
{
	char *p;

	if (myOsQueueEmpty(q))
		return(-1);
	p=(char*)q->pQueue + (q->first*q->data_size);
	memcpy(data,p,q->data_size);
	if (q->first == q->last)
		q->first = q->last = -1;
	else
		q->first = (q->first+1) % q->queue_size;
	return(0);	
}

