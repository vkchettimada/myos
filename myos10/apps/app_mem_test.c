/* app_radio.c */
#include <stdlib.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

typedef struct _llist_t_ {
	int data;
	struct _llist_t_ *next;
} llist_t;

static llist_t *start;

static int llist_insert(llist_t **head, int data)
{
	llist_t *pnew;

	pnew = (llist_t*)malloc(sizeof(llist_t));
	if (!pnew)
		return(-1);
	pnew->data = data;
	pnew->next = *head;
	*head = pnew;
	return(0);
}

static void llist_remove(llist_t **head, int data)
{
	llist_t *p1, *p2;

	p1 = *head;
	p2 = p1;
	while (p1 && p1->data != data) {
		p2 = p1;
		p1 = p1->next;
	}
	if (!p1) {
		return;
	}
	if (p2 == p1) {
		*head = p1->next;
	} else {
		p2->next = p1->next;
	}
	free(p1);
	return;
}

static void llist_disp(llist_t *head)
{
	console_printf("list -> ");
	while (head) {
		console_printf("%d ", head->data);
		head = head->next;
	}
	console_printf("NULL\n");
}

static int app_mem_test_handler(char *s)
{
	
	console_printf("%s\n", s);

	switch (s[0])
	{
		case 'i':
			llist_insert(&start, atoi(&s[1]));
			break;
		case 'r':
			llist_remove(&start,atoi(&s[1]));
			break;
		default:
			llist_disp(start);
			break;
	}

	return(0);
}

void app_mem_test_task(void)
{
	console_create("mem", CONSOLE_FLAG_ECHO | CONSOLE_FLAG_STRING, app_mem_test_handler);
	
	while(1) {
		myOsSleep(1000);
	}
}

