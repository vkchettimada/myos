/* myos.h */

#ifndef _MYOS_H_
#define _MYOS_H_

#ifdef MYOS_DEBUG
#undef MYOS_DEBUG
#define MYOS_DEBUG printk
#else
#define MYOS_DEBUG
#endif

#define MYOS_MAX_TASK 10 // Max no. of tasks allowed
#define MYOS_SP_START 0x10a000 // SP initial value
#define MYOS_IP_LEN 4 // IP length
#define MYOS_SP_DIR (-1) // SP grow direction

typedef void (*MyOsTask)(void); // Task prototype
typedef struct
{
	int priority;
	int sleep;
	int count;
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

void myOsInit();
int myOsAddTask(MyOsTask taskAddr, int stackSize, int priority);
int myOsGetCurrTask(void);
void myOsSwitch(int eligible);
void myOsDelay(int sleep);
void myOsStart();

#endif

