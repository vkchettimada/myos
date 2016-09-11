/* cntl.h */
#ifndef _CNTL_H_
#define _CNTL_H_

typedef struct 
{
	int value;
	int deflt;
} cntl_value_bool_t;

typedef struct 
{
	int value;
	int deflt;
	int min;
	int max;
} cntl_value_range_t;

typedef union 
{
	cntl_value_bool_t boole;
	cntl_value_range_t range;
} cntl_value_t;

typedef struct _cntl_t_
{
	char name[50];
	int type;
#define CNTL_TYPE_BOOL 0x01
#define CNTL_TYPE_RANGE 0x02
	cntl_value_t value;
	struct _cntl_t_ *next;
} cntl_t;

#endif

