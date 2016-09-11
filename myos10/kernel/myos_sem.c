#include <myos/myos.h>
#include <myos/myos_sem.h>

void myOsSemWait(MyOsSem *s)
{
	int curr, eligible;
	
	s->value -= 1;
	if (s->value < 0) {
		curr = myOsGetCurrTask();
		myOsQueueAdd(s->pQueuedTasks, &curr);
		myOsContext[curr].state = task_waiting;
		eligible = myOsEligible();
		myOsContext[eligible].state = task_running;
		myOsSwitch(eligible);
	}
}

void myOsSemSignal(MyOsSem *s)
{
	int curr, queued;
	
	s->value+=1;
	if (s->value<=0) {
		if (!myOsQueueRemove(s->pQueuedTasks, &queued)) {
			myOsContext[queued].state = task_running;
			curr = myOsGetCurrTask();
			myOsContext[curr].state = task_ready;
			myOsSwitch(queued);
		}
	}
}

int myOsSemPending(MyOsSem *s)
{
	return(s->value<0);
}

