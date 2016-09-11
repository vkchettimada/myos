#include <asm/delay.h>
#include <myos/myos.h>
#include <myos/myos_irq.h>

static unsigned int myOsStackEnd; // Stack top addr at any instance
static int myOsTasks; // no. of running tasks
static int myOsCurrTask; // Current running task
context_t myOsContext[MYOS_MAX_TASK];

void myOsIdle(void)
{
	while(1) {
		MYOS_DEBUG("idle ");
		myOsSleep(0);
	}
}

void myOsTimer(int irq, regs_t *regs)
{
	int i;

	for (i=0;i<myOsTasks;i++) {
		if (myOsContext[i].sleep > 0)
			myOsContext[i].sleep--;
		else {
			if (myOsContext[i].state == task_sleeping)
				myOsContext[i].state = task_ready;
			else if (myOsContext[i].state == task_running && myOsContext[i].count > 0)
				myOsContext[i].count--;
		}
	}
}

int myOsEligible()
{
	int i,p=0,eligible=0;

	for (i=0;i<myOsTasks;i++) {
		if (myOsContext[i].state == task_ready) {
			if (myOsContext[i].priority>p) {
				eligible=i;
				p=myOsContext[i].priority;
			} else if (myOsContext[i].priority==p
					&& myOsContext[i].count>myOsContext[eligible].count) {
				eligible=i;
			}
		}
	}
	return(eligible);
}

void myOsSwitch(int eligible)
{
	int BP,AX,BX,CX,DX,SI;
	
	MYOS_DEBUG("myOsSwitch\n");
			
	__asm__ __volatile__ ("mov %%ebp,%0\n \
			mov %%eax,%1\n \
			mov %%ebx,%2\n \
			mov %%ecx,%3\n \
			mov %%edx,%4\n \
			mov %%esi,%5\n"
			:"=m" (BP)
			,"=m" (AX)
			,"=m" (BX)
			,"=m" (CX)
			,"=m" (DX)
			,"=m" (SI)
			:);
	myOsContext[myOsCurrTask].SP=BP+8;
	myOsContext[myOsCurrTask].IP=*((unsigned int*)(BP+4));
	myOsContext[myOsCurrTask].BP=*((unsigned int*)BP);
	myOsContext[myOsCurrTask].DI=*((unsigned int*)(BP-4));
	myOsContext[myOsCurrTask].AX=AX;
	myOsContext[myOsCurrTask].BX=BX;
	myOsContext[myOsCurrTask].CX=CX;
	myOsContext[myOsCurrTask].DX=DX;
	myOsContext[myOsCurrTask].SI=SI;
	
	MYOS_DEBUG("switching: %d (%x) to %d (%x)\n",
			myOsCurrTask, myOsContext[myOsCurrTask].IP,
			eligible, myOsContext[eligible].IP);
	
	if (myOsContext[myOsCurrTask].count==0 && myOsCurrTask!=0)
		myOsContext[myOsCurrTask].count=100;
	myOsCurrTask=eligible;
	__asm__ __volatile__ ("mov %0,%%esi\n \
			mov %1,%%edi\n \
			mov %2,%%edx\n \
			mov %3,%%ecx\n \
			mov %4,%%ebx\n \
			mov %6,%%ebp\n \
			mov %7,%%esp\n \
			push %8\n \
			mov %5,%%eax\n \
			ret"
			:
			: "m" (myOsContext[myOsCurrTask].SI),
			"m" (myOsContext[myOsCurrTask].DI),
			"m" (myOsContext[myOsCurrTask].DX),
			"m" (myOsContext[myOsCurrTask].CX),
			"m" (myOsContext[myOsCurrTask].BX),
			"m" (myOsContext[myOsCurrTask].AX),
			"m" (myOsContext[myOsCurrTask].BP),
			"m" (myOsContext[myOsCurrTask].SP),
			"m" (myOsContext[myOsCurrTask].IP));
}

void myOsInit()
{
	extern unsigned int _end;
	
	MYOS_DEBUG("myOsInit\n");
	myOsStackEnd = (unsigned int)&_end;
	myOsTasks=myOsCurrTask=0;
}

int myOsAddTask(MyOsTask taskAddr, int stackSize, int priority)
{
	MYOS_DEBUG("myOsAddTask\n");
	if (myOsTasks<MYOS_MAX_TASK)
	{
		myOsContext[myOsTasks].IP=(unsigned int)taskAddr;
		myOsStackEnd+=stackSize;
		myOsContext[myOsTasks].SP=myOsStackEnd;
		myOsContext[myOsTasks].priority=priority;
		myOsContext[myOsTasks].sleep=0;
		myOsContext[myOsTasks].count=0;
		myOsContext[myOsTasks].state=task_ready;
		myOsTasks++;
		return(myOsTasks);
	}
	return(-1);
}

void myOsStart()
{
	MYOS_DEBUG("myOsStart\n");

	__asm__ __volatile__ ("mov %0,%%esp\n \
			push %1\n \
			ret"
			:
			: "m" (myOsContext[myOsCurrTask].SP),
			"m" (myOsContext[myOsCurrTask].IP));
}

int myOsGetCurrTask(void)
{
	return(myOsCurrTask);
}

void myOsSleep(int sleep)
{
	int eligible;
	
	MYOS_DEBUG("myOsSleep\n");
	myOsContext[myOsCurrTask].state = task_sleeping;
	myOsContext[myOsCurrTask].sleep = sleep;
	eligible = myOsEligible();
	myOsContext[eligible].state = task_running;
	myOsSwitch(eligible);
}

