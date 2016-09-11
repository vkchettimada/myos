/* myos.h */

#ifndef _MYOS_H_
#define _MYOS_H_

#ifdef ARCH_I386
	#include <asm/i386.h>
#endif

//#define MYOS_DEBUG printk

#ifndef MYOS_DEBUG
#define MYOS_DEBUG myOsDummyDebug
#endif

#define MYOS_MAX_TASK 10 // max. no. of tasks
#define MYOS_SP_START end // SP initial value
#define MYOS_IP_LEN 4 // IP length
#define MYOS_SP_DIR (-1) // SP grow direction

typedef enum
{
	task_idle,
	task_ready,
	task_running,
	task_sleeping,
	task_waiting,
	task_stopped
} MyOsTaskState;

typedef void (*MyOsTask)(void); // Task prototype
typedef struct
{
	int priority;
	int sleep;
	int count;
	MyOsTaskState state;
	unsigned int IP;
	unsigned int SP;
	unsigned int BP;
	unsigned int AX;
	unsigned int BX;
	unsigned int CX;
	unsigned int DX;
	unsigned int DI;
	unsigned int SI;
}context_t; // Struct to store context

extern context_t myOsContext[MYOS_MAX_TASK];

extern unsigned int myOsTick;

void myOsDummyDebug(char *s, ...);
void myOsIdle(void);
void myOsTimer(int irq, regs_t *regs);
int myOsEligible();
void myOsSwitch(int eligible);
void myOsInit();
int myOsAddTask(MyOsTask taskAddr, int stackSize, int priority);
int myOsGetCurrTask(void);
void myOsSleep(int sleep);
void myOsStart();

#endif

