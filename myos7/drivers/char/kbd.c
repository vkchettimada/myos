/* kbd.c */
#include <stdio.h>
#include <driver/i8259a.h>
#include <driver/kbd.h>
#include <myos/myos_irq.h>
#include <myos/myos_io.h>
#include <myos/myos_globals.h>

#define KBD_DATA 0x60

static const unsigned char kbd_map[] =	{
	/* 00 */0,	0x1B,	'1',	'2',	'3',	'4',	'5',	'6',
	/* 08 */'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
	/* 10 */'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
	/* 1Dh is left Ctrl */
	/* 18 */'o',	'p',	'[',	']',	'\n',	0,	'a',	's',
	/* 20 */'d',	'f',	'g',	'h',	'j',	'k',	'l',	';',
	/* 2Ah is left Shift */
	/* 28 */'\'',	'`',	0,	'\\',	'z',	'x',	'c',	'v',
	/* 36h is right Shift */
	/* 30 */'b',	'n',	'm',	',',	'.',	'/',	0,	0,
	/* 38h is left Alt, 3Ah is Caps Lock */
	/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
	/* 45h is Num Lock, 46h is Scroll Lock */
	/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
	/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
	/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
	/* 58 */KEY_F12
};

static unsigned char kbd_ascii(unsigned char s)
{
	static int extended=0;

	if (extended) {
		extended=0;
		return(0);
	}

	if (s==0xe0) {
		extended=1;
		return(0);
	}

	if (s>=0x80)
		return(0);

	if (s >= sizeof(kbd_map)/sizeof(kbd_map[0]))
	       return(0);

	return(kbd_map[s]);
}

static void kbd_irq(int irq, regs_t *regs)
{
	unsigned char s,c;
	s=inpb(KBD_DATA);
	c=kbd_ascii(s);

	if (c)
		myOsQueueAdd(&queue_kbd,&c);

	i8259a_eoi();
}

void kbd_init(void)
{
	printk("kbd_init\n");
	myOsIrqRegister(0x21,kbd_irq);
	i8259a_enable(1);
}

