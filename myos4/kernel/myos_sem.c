#include <myos/myos.h>
#include <myos/myos_sem.h>

void myos_wait(MyOsSem *s)
{
	s->task=myOsGetCurrTask();
	while (!s->value) {
		myOsDelay(1);
	}
	s->value=0;
	s->task=0;
}

void myos_signal(MyOsSem *s)
{
	s->value=1;
	if (s->task!=0)
	{
//		printk("switching to %d\n",s->task);
//		myOsSwitch(s->task);
	}
}

