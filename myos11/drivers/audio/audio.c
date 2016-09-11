/* audio.c */
#include <stdio.h>
#include <string.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/pci.h>
#include <driver/pci_ids.h>
#include <driver/lpc.h>
#include <driver/i8259a.h>
#include <myos/myos_irq.h>

/* AC97 NAMBAR registers */
#define AC97_NAMBAR_REG_RESET      0x0000
#define AC97_NAMBAR_REG_MASTER_VOL 0x0002
#define AC97_NAMBAR_REG_MIC_VOL    0x000E
#define AC97_NAMBAR_REG_LINEIN_VOL 0x0010
#define AC97_NAMBAR_REG_PCMOUT_VOL 0x0018
#define AC97_NAMBAR_REG_REC_GAIN   0x001C
#define AC97_NAMBAR_REG_POWERDOWN  0x0026
#define AC97_NAMBAR_REG_EXTENDCR   0x002A
#define AC97_NAMBAR_REG_PCMDACRR   0x002C
#define AC97_NAMBAR_REG_VENDOR_ID1 0x007C
#define AC97_NAMBAR_REG_VENDOR_ID2 0x007E

/* AC97 NABMBAR registers */
#define AC97_NABMBAR_REG_POBDBAR   0x0010
#define AC97_NABMBAR_REG_POCIV     0x0014
#define AC97_NABMBAR_REG_POLVI     0x0015
#define AC97_NABMBAR_REG_POSR      0x0016
#define AC97_NABMBAR_REG_POPIV     0x001A
#define AC97_NABMBAR_REG_POCR      0x001B
#define AC97_NABMBAR_REG_GLOB_CNT  0x002C
#define AC97_NABMBAR_REG_GLOB_STA  0x0030
#define AC97_NABMBAR_REG_CAS       0x0034

/* AC97 PCMOUT, PCMIN, MICIN status register flags */
#define AC97_xSR_FIFOE (1<<4)
#define AC97_xSR_BCIS  (1<<3)
#define AC97_xSR_LVBCI (1<<2)
#define AC97_xSR_CELV  (1<<1)
#define AC97_xSR_DCH   (1<<0)

/* AC97 PCMOUT, PCMIN, MICIN control register flags */
#define AC97_xCR_IOCE  (1<<4)
#define AC97_xCR_FEIE  (1<<3)
#define AC97_xCR_LVBIE (1<<2)
#define AC97_xCR_RR    (1<<1)
#define AC97_xCR_RUN   (1<<0)

/* AC97 global control register flags */
#define AC97_GLOB_CNT_COLD_RESET (1<<1)
#define AC97_GLOB_CNT_WARM_RESET (1<<2)

/* AC97 global status register flags */
#define AC97_GLOB_STA_RCS   (1<<15)
#define AC97_GLOB_STA_SCR   (1<<9)
#define AC97_GLOB_STA_PCR   (1<<8)
#define AC97_GLOB_STA_MCINT (1<<7)
#define AC97_GLOB_STA_POINT (1<<6)
#define AC97_GLOB_STA_PIINT (1<<5)

/* AC97 chip access semaphore register flags */
#define AC97_CAS (1<<0)

#define ac97_nambar_outw(data,port)	ac97_checked_nambar_outw(ac97_inst, data, port)
#define ac97_nambar_inw(port)		ac97_checked_nambar_inw(ac97_inst, port)

#define ac97_nabmbar_outb(data,port)	outpb(data,ac97_inst->nabmbar+port)
#define ac97_nabmbar_outl(data,port)	outpl(data,ac97_inst->nabmbar+port)
#define ac97_nabmbar_inb(port)		inpb(ac97_inst->nabmbar+port)
#define ac97_nabmbar_inl(port)		inpl(ac97_inst->nabmbar+port)


#define AC97_BD_MAX 32
#define AC97_BUFFER_LEN (64*1024)

struct ac97_bd_t {
	unsigned int buffer_ptr;
	unsigned int buffer_len:16;
	unsigned int reserved1:14;
	unsigned int BUP:1;
	unsigned int IOC:1;
};

struct ac97_inst_t {
	struct pci_dev_t *dev;
	unsigned short nambar;
	unsigned short nabmbar;

	unsigned short vendor_id1;
	unsigned short vendor_id2;
};

static struct ac97_bd_t __attribute__((aligned(4))) mic_in_list[AC97_BD_MAX];
static struct ac97_bd_t __attribute__((aligned(4))) pcm_in_list[AC97_BD_MAX];
static struct ac97_bd_t __attribute__((aligned(4))) pcm_out_list[AC97_BD_MAX];
static unsigned char __attribute__((aligned(4))) mic_in_buffer[AC97_BD_MAX][AC97_BUFFER_LEN];
static unsigned char __attribute__((aligned(4))) pcm_in_buffer[AC97_BD_MAX][AC97_BUFFER_LEN];
static unsigned char __attribute__((aligned(4))) pcm_out_buffer[AC97_BD_MAX][AC97_BUFFER_LEN];

static struct ac97_inst_t __attribute__((aligned(4))) ac97_instance;

/********************************************************************
 *
 *******************************************************************/

static int ac97_cas(struct ac97_inst_t *ac97_inst)
{
	int timeout = 100;

	do {
		if (!(ac97_nabmbar_inl(AC97_NABMBAR_REG_CAS) & AC97_CAS))
			return(0);
		udelay(10);
	} while (timeout--);
	printk("ac97: sema locked\n");
	return(-1);
}

static void ac97_checked_nambar_outw(struct ac97_inst_t *ac97_inst
		, unsigned short data, unsigned short port)
{
	if (!ac97_cas(ac97_inst))
		outpw(data,ac97_inst->nambar+port);
}

static unsigned short ac97_checked_nambar_inw(struct ac97_inst_t *ac97_inst
		, unsigned short port)
{
	unsigned short data=0;

	if (!ac97_cas(ac97_inst))
		data = inpw(ac97_inst->nambar+port);
	return(data);
}

/********************************************************************
 *
 *******************************************************************/

static int ac97_reset(struct ac97_inst_t *ac97_inst, int cold)
{
	int timeout;
	unsigned int cnt, status;

	if (cold) {
		ac97_nabmbar_outl(0, AC97_NABMBAR_REG_GLOB_CNT);
	}
	
	status = ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_STA);
	ac97_nabmbar_outl( (status & (AC97_GLOB_STA_RCS | AC97_GLOB_STA_MCINT | AC97_GLOB_STA_POINT | AC97_GLOB_STA_PIINT))
			, AC97_NABMBAR_REG_GLOB_STA);
		
	cnt = ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_CNT);
	cnt |= ((cnt & AC97_GLOB_CNT_COLD_RESET) == 0) ? AC97_GLOB_CNT_COLD_RESET : AC97_GLOB_CNT_WARM_RESET;
	ac97_nabmbar_outl(cnt, AC97_NABMBAR_REG_GLOB_CNT);
	timeout = timer_ticks + HZ/4;
	do {
		cnt = ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_CNT);
		if ( (cnt & AC97_GLOB_CNT_WARM_RESET) == 0 )
			goto __ok;
		udelay(100000);
	} while (timer_ticks < timeout);
	printk("ac97: reset timeout, glob_cnt= 0x%x\n", cnt);
	return(-1);
__ok:
	/* wait for any codec ready status.
	 * Once it becomes ready it should remain ready
	 * as long as we do not disable the ac97 link.
	 */
	timeout = timer_ticks + HZ;
	do {
		status = ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_STA) & (AC97_GLOB_STA_SCR | AC97_GLOB_STA_PCR);
		if (status)
			break;
		udelay(100000);
	} while (timer_ticks < timeout);
	if (! status) {
		/* no codec is found */
		printk("ac97: codec is not ready [0x%x]\n", ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_STA));
		return(-1);
	}
	
	return(0);
}

static int ac97_ad1881_reset(struct ac97_inst_t *ac97_inst)
{
	int timeout;
	unsigned short rec_gain;

	ac97_nambar_outw(0,AC97_NAMBAR_REG_RESET);

	timeout = timer_ticks + HZ/2;
	do {
		ac97_nambar_inw(AC97_NAMBAR_REG_RESET);
		ac97_nambar_inw(AC97_NAMBAR_REG_VENDOR_ID1);
		ac97_nambar_inw(AC97_NAMBAR_REG_VENDOR_ID2);

		ac97_nambar_outw(0x8a05, AC97_NAMBAR_REG_REC_GAIN);
		rec_gain = ac97_nambar_inw(AC97_NAMBAR_REG_REC_GAIN);
		if ( (rec_gain&0x7fff) == 0x0a05 )
			return(0);
		udelay(100000);
	} while ( timer_ticks < timeout );
	printk("ad1881: reset failed\n");
	return(-1);
}

static int ac97_ad1881_init(struct ac97_inst_t *ac97_inst)
{
	ac97_nambar_outw(0x0000,AC97_NAMBAR_REG_MASTER_VOL);
	ac97_nambar_outw(0x0808,AC97_NAMBAR_REG_MIC_VOL);
	ac97_nambar_outw(0x0808,AC97_NAMBAR_REG_LINEIN_VOL);
	ac97_nambar_outw(0x0808,AC97_NAMBAR_REG_PCMOUT_VOL);
	ac97_nambar_outw(0x0001,AC97_NAMBAR_REG_EXTENDCR);
	ac97_nambar_outw(0xAC44,AC97_NAMBAR_REG_PCMDACRR);
	return(0);
}

static int ac97_bd_init(struct ac97_inst_t *ac97_inst)
{
	int index;

	for (index=0; index <AC97_BD_MAX; index++) {
		mic_in_list[index].buffer_ptr = 
			(unsigned int) mic_in_buffer[index];
		mic_in_list[index].buffer_len = AC97_BUFFER_LEN/2;
		mic_in_list[index].IOC = 1;
		mic_in_list[index].BUP = 1;
		pcm_in_list[index].buffer_ptr = 
			(unsigned int) pcm_in_buffer[index];
		pcm_in_list[index].buffer_len = AC97_BUFFER_LEN/2;
		pcm_in_list[index].IOC = 1;
		pcm_in_list[index].BUP = 1;
		pcm_out_list[index].buffer_ptr = 
			(unsigned int) pcm_out_buffer[index];
		pcm_out_list[index].buffer_len = AC97_BUFFER_LEN/2;
		pcm_out_list[index].IOC = 1;
		pcm_out_list[index].BUP = 0;
	}

	/* Init mic in */

	/* Init pcm in */
	
	/* Init pcm out */
	ac97_nabmbar_outl((unsigned long)pcm_out_list
			, AC97_NABMBAR_REG_POBDBAR);
	ac97_nabmbar_outb(0, AC97_NABMBAR_REG_POCIV);
	ac97_nabmbar_outb(AC97_BD_MAX-1, AC97_NABMBAR_REG_POLVI);
	
	return(0);
}

static int ac97_start(struct ac97_inst_t *ac97_inst, int flag)
{
	unsigned char cnt;

	cnt = AC97_xCR_IOCE;

	if (flag)
		cnt |= AC97_xCR_RUN;

	ac97_nabmbar_outb(cnt, AC97_NABMBAR_REG_POCR);
}

static void audio_irq(int irq, regs_t *regs)
{
	struct ac97_inst_t *ac97_inst = &ac97_instance;
	unsigned int status, mask;
	unsigned char sr;

	status = ac97_nabmbar_inl(AC97_NABMBAR_REG_GLOB_STA);
	if (status == 0xffffffffUL)	/* we are not yet resumed */
		goto eoi;
	
	mask = AC97_GLOB_STA_RCS
		| AC97_GLOB_STA_MCINT
		| AC97_GLOB_STA_POINT
		| AC97_GLOB_STA_PIINT;
	
	if ((status & mask) == 0) {
		if (status) {
			printk("audio: spurious interrupt\n");
			ac97_nabmbar_outl(status, AC97_NABMBAR_REG_GLOB_STA);
		}
		goto eoi;
	}

	//printk("audio_irq\n");

	/* Check and handle MICINT
	 *
	 * */

	/* Check and handle POINT */
	sr = ac97_nabmbar_inb(AC97_NABMBAR_REG_POSR);	
	ac97_nabmbar_outb(sr, AC97_NABMBAR_REG_POSR);

	/* Check and handle PIINT
	 *
	 * */

	/* acknowledge interrupt */
	ac97_nabmbar_outl(status & mask, AC97_NABMBAR_REG_GLOB_STA);
eoi:
	/* End of Interrupt */
	i8259a_eoi();
}

int audio_play(short *data, int *len)
{
	struct ac97_inst_t *ac97_inst = &ac97_instance;
	int lvi, civ;
	
	civ = ac97_nabmbar_inb(AC97_NABMBAR_REG_POCIV);
	lvi = ac97_nabmbar_inb(AC97_NABMBAR_REG_POLVI);
	lvi = (lvi+1)%AC97_BD_MAX;
	if (lvi == civ)
	{
		*len = 0;
		return(-1);
	}
	if (*len > AC97_BUFFER_LEN)
		*len = AC97_BUFFER_LEN;
	memcpy((void*)pcm_out_list[lvi].buffer_ptr, data, *len);
	pcm_out_list[lvi].buffer_len = *len/2;
	ac97_nabmbar_outb(lvi, AC97_NABMBAR_REG_POLVI);
	return(0);
}

void audio_init(void)
{
	struct ac97_inst_t *ac97_inst = &ac97_instance;

	ac97_inst->dev = pci_get_device(VENDOR_INTEL, DEVICE_ICH_AC97);
	if (!ac97_inst->dev) {
		printk("audio: device not found.\n");
		return;
	}

	ac97_inst->nambar = ac97_inst->dev->base_addr[0] & (~0xf);
	ac97_inst->nabmbar = ac97_inst->dev->base_addr[1] & (~0xf);

	printk("audio: vendor= 0x%x device= 0x%x nambar= 0x%x nabmbar= 0x%x\n"
			, ac97_inst->dev->ven_id 
			, ac97_inst->dev->dev_id 
			, ac97_inst->nambar
			, ac97_inst->nabmbar);

	pci_set_master(ac97_inst->dev);
	myOsIrqRegister(0x20+ac97_inst->dev->irq, audio_irq);
	i8259a_enable(ac97_inst->dev->irq, 1);
	lpc_enable_pirq(ac97_inst->dev->irq_pin);
		
	/* cold reset ac97 component */	
	if (ac97_reset(ac97_inst, 1)) {
		printk("ac97: ac97 reset failed\n");
		return;
	}

	if (ac97_ad1881_reset(ac97_inst)) {
		printk("ac97: ad1881 reset failed\n");
		return;
	}
	
	ac97_ad1881_init(ac97_inst);
	ac97_bd_init(ac97_inst);
	ac97_start(ac97_inst, 1);
}

