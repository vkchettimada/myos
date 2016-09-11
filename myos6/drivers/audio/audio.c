/* audio.c */
#include <stdio.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/pci.h>

#define VENDOR_INTEL 0x8086
#define DEVICE_ICH 0x2415

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
#define AUDIO_BUFFER_LEN (8*1024)


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

	struct ac97_bd_t pcm_out_list[AC97_BD_MAX];
	unsigned short pcm_buffer[AC97_BD_MAX][AUDIO_BUFFER_LEN];
};

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

	printk("ad1881: reset= 0x%x\n", ac97_nambar_inw(AC97_NAMBAR_REG_RESET) );
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

	ac97_nambar_outw(0x0000,AC97_NAMBAR_REG_MASTER_VOL); /* 0 is max = 0 dB */
	ac97_nambar_outw(0x0808,AC97_NAMBAR_REG_MIC_VOL);

	return(0);
}

void audio_init(void)
{
	struct ac97_inst_t ac97_instance;
	struct ac97_inst_t *ac97_inst = &ac97_instance;

	ac97_inst->dev = pci_get_device(VENDOR_INTEL, DEVICE_ICH);
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
		
	/* cold reset ac97 component */	
	if (ac97_reset(ac97_inst, 1)) {
		printk("ac97: ac97 reset failed\n");
		return;
	}

	printk("ac97: ac97 reset complete\n");

	if (ac97_ad1881_reset(ac97_inst)) {
		printk("ac97: ad1881 reset failed\n");
		return;
	}
	
	printk("ac97: ad1881 reset complete\n");

	ac97_ad1881_init(ac97_inst);
}

