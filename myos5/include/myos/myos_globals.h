#ifndef _MYOS_GLOBALS_H_
#define _MYOS_GLOBALS_H_

#ifndef _MYOS_GLOBALS_C_
	#define EXTERN extern
#else
	#define EXTERN
#endif

#include <myos/myos.h>
#include <myos/myos_sem.h>
#include <myos/myos_queue.h>

/********************************************************************
 * Tasks
 * ******************************************************************/

typedef struct
{
	char name[50];

	MyOsTask entry;
	int stack_size;
	int priority;
} task_t;

#ifdef MYOS_DEFINE_TASK
	#undef MYOS_DEFINE_TASK
#endif
#define MYOS_DEFINE_TASK(name,entry,stack_size,priority) MYOSTASK_##entry,
EXTERN enum {
	MYOSTASK_IDLE_TASK,
	#include <myos_task.def>
	MYOS_N_TASKS
} ENUM_MYOSTASK_ID;

#ifdef MYOS_DEFINE_TASK
	#undef MYOS_DEFINE_TASK
#endif
#define MYOS_DEFINE_TASK(name,entry,stack_size,priority) \
	void entry(void);
#include <myos_task.def>

#ifdef MYOS_DEFINE_TASK
	#undef MYOS_DEFINE_TASK
#endif
#define MYOS_DEFINE_TASK(name,entry,stack_size,priority) \
	,{#name,entry,stack_size,priority}
EXTERN task_t Tasks[MYOS_N_TASKS]
	#ifdef _MYOS_GLOBALS_C_
		= {
		{"Idle task",myOsIdle,4096,0}
		#include <myos_task.def>
		};
	#else
		;
	#endif

/********************************************************************
 * Semaphores
 * ******************************************************************/

typedef struct
{
	MyOsSem *s;
	char name[50];
	char description[50];
} sem_t;

#ifdef MYOS_DEFINE_SEMAPHORE
	#undef MYOS_DEFINE_SEMAPHORE
#endif
#define MYOS_DEFINE_SEMAPHORE(name,description) MYOSSEM_##name,
EXTERN enum {
	#include <myos_sem.def>
	MYOS_N_SEMS
} ENUM_MYOSSEM_ID;

#ifdef MYOS_DEFINE_SEMAPHORE
	#undef MYOS_DEFINE_SEMAPHORE
#endif
#ifdef _MYOS_GLOBALS_C_
	#define MYOS_DEFINE_SEMAPHORE(name,description) \
		EXTERN char MYOSQUEUE_DATA_SEMA_##name[sizeof(int)*MYOS_MAX_TASK]; \
		EXTERN MyOsQueue MYOSQUEUE_SEMA_##name \
				= { \
				MYOSQUEUE_DATA_SEMA_##name, \
				sizeof(int), \
				MYOS_MAX_TASK, \
				-1, \
				-1, \
				};
#else
	#define MYOS_DEFINE_SEMAPHORE(name,description) \
		EXTERN char MYOSQUEUE_DATA_SEMA_##name[sizeof(int)*MYOS_MAX_TASK]; \
		EXTERN MyOsQueue MYOSQUEUE_SEMA_##name;
#endif
#include <myos_sem.def>

#ifdef MYOS_DEFINE_SEMAPHORE
	#undef MYOS_DEFINE_SEMAPHORE
#endif
#ifdef _MYOS_GLOBALS_C_
	#define MYOS_DEFINE_SEMAPHORE(name,description) \
		EXTERN MyOsSem name = { \
			0, \
			&MYOSQUEUE_SEMA_##name \
		};
#else
	#define MYOS_DEFINE_SEMAPHORE(name,description) EXTERN MyOsSem name;
#endif
#include <myos_sem.def>


#ifdef MYOS_DEFINE_SEMAPHORE
	#undef MYOS_DEFINE_SEMAPHORE
#endif
#define MYOS_DEFINE_SEMAPHORE(name,description) \
	{&name, #name, #description},
EXTERN sem_t Semaphores[MYOS_N_SEMS]
	#ifdef _MYOS_GLOBALS_C_
		= {
			#include <myos_sem.def>
		};
	#else
		;
	#endif

/********************************************************************
 * Queues
 * ******************************************************************/

typedef struct
{
	MyOsQueue *q;
	char name[50];
	char description[50];
} queue_t;

#ifdef MYOS_DEFINE_QUEUE
	#undef MYOS_DEFINE_QUEUE
#endif
#define MYOS_DEFINE_QUEUE(name,description,data_size,queue_size) \
	MYOSQUEUE_##name,
EXTERN enum {
	#include <myos_queue.def>
	MYOS_N_QUEUES
} ENUM_MYOSQUEUE_ID;

#ifdef MYOS_DEFINE_QUEUE
	#undef MYOS_DEFINE_QUEUE
#endif
#define MYOS_DEFINE_QUEUE(name,description,data_size,queue_size) \
	EXTERN char MYOSQUEUE_DATA_##name[data_size*queue_size];
#include <myos_queue.def>

#ifdef MYOS_DEFINE_QUEUE
	#undef MYOS_DEFINE_QUEUE
#endif
#ifdef _MYOS_GLOBALS_C_
	#define MYOS_DEFINE_QUEUE(name,description,data_size,queue_size) \
		EXTERN MyOsQueue name \
				= { \
				MYOSQUEUE_DATA_##name, \
				data_size, \
				queue_size, \
				-1, \
				-1, \
				};
#else
	#define MYOS_DEFINE_QUEUE(name,description,data_size,queue_size) \
		EXTERN MyOsQueue name;
#endif
#include <myos_queue.def>
			
#ifdef MYOS_DEFINE_QUEUE
	#undef MYOS_DEFINE_QUEUE
#endif
#define MYOS_DEFINE_QUEUE(name,description,data_size,queue_size) \
	{&name, #name, #description},
EXTERN queue_t Queues[MYOS_N_QUEUES]
	#ifdef _MYOS_GLOBALS_C_
		= {
			#include <myos_queue.def>
		};
	#else
		;
	#endif

/********************************************************************/
#endif
