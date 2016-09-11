/* malloc.c */
#include <stdio.h>

typedef struct _malloc_t_{
	void *p;
	int size;
	struct _malloc_t_ *next;
} malloc_t;

static void *malloc_mem_beg = (void*)0x1e00000;
static void *malloc_mem_end = (void*)0x1f00000;
static malloc_t *malloc_head=0;

void malloc_init(void *mem, int size)
{
	malloc_mem_beg = mem;
	malloc_mem_end = (void*)((char*)mem + size);
}

/*
void *malloc(int size)
{
	malloc_t *plist, *pnews, *pnewe, *ps, *pe;
	
	if (size<=0)
		return(0);
	
	plist = malloc_head;
	pnews = malloc_mem_beg;
	pnewe = (malloc_t*)(((char*)pnews) + size + sizeof(malloc_t));
	while (plist) {
		printk("malloc: pnews= 0x%x\n", pnews);
		ps = (malloc_t*)plist->p;
		pe = (malloc_t*)(((char*)plist->p) + plist->size + sizeof(malloc_t));
		if ( ((pnews >= ps) && (pnews < pe))
				|| ( (pnewe >= ps) && (pnewe < pe) ) ) {
			pnews = pe;
			pnewe = (malloc_t*)(((char*)pnews) + size +sizeof(malloc_t));
			plist = malloc_head;
			continue;
		}
		plist = plist->next;
	}

	printk("malloc: pnews= 0x%x\n", pnews);
	pnews->p = ((char*)pnews)+sizeof(malloc_t);
	pnews->size = size;
	pnews->next = malloc_head;
	malloc_head = pnews;

	return(pnews->p);
}
*/

void *malloc(int size)
{
	malloc_t *plist, *pnews, *pnewe, *ps, *pe;
	
	if (size<=0)
		return(0);
	
	plist = malloc_head;
	pnewe = (malloc_t*) malloc_mem_end;
	pnews = (malloc_t*)((char*)pnewe - size - sizeof(malloc_t));
	while (plist) {
		printk("malloc: pnews= 0x%x\n", pnews);
		ps = plist;
		pe = (malloc_t*)((char*)ps + plist->size + sizeof(malloc_t));
		if ( ((pnews >= ps) && (pnews < pe))
				|| ( (pnewe > ps) && (pnewe <= pe) ) ) {
			pnewe = ps;
			pnews = (malloc_t*)((char*)pnewe - size - sizeof(malloc_t));
			plist = malloc_head;
			continue;
		}
		plist = plist->next;
	}

	printk("malloc: pnews= 0x%x\n", pnews);
	pnews->p = (void*)(pnews+1);
	pnews->size = size;
	pnews->next = malloc_head;
	malloc_head = pnews;

	return(pnews->p);
}

void free(void *p)
{
	malloc_t *p1, *p2;

	p1=malloc_head;
	p2=p1;
	while ( p1 && p1->p != p) {
		p2 = p1;
		p1 = p1->next;	
	}
	if (!p1) {
		return;
	}
	if (p2 == p1) {
		malloc_head = p1->next;
		return;
	}
	p2->next = p1->next;
}

