/* i2c.c */
#include <stdio.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/i2c.h>

#define setscl(adap, val) adap->setscl(adap->data, val)
#define setsda(adap, val) adap->setsda(adap->data, val)
#define getscl(adap) adap->getscl(adap->data)
#define getsda(adap) adap->getsda(adap->data)

static inline int sclhi(struct i2c_adaptor_t *adap)
{
	int timeout;

	setscl(adap, 1);

	timeout= timer_ticks + adap->timeout;
	do {
		if (getscl(adap)) {
			udelay(adap->udelay);
			return(0);
		}
	} while (timer_ticks < timeout);

	return(-1);
}

static inline void scllo(struct i2c_adaptor_t *adap)
{
	setscl(adap, 0);
	udelay(adap->udelay);
}

static inline void sdahi(struct i2c_adaptor_t *adap)
{
	setsda(adap, 1);
	udelay(adap->udelay);
}

static inline void sdalo(struct i2c_adaptor_t *adap)
{
	setsda(adap, 0);
	udelay(adap->udelay);
}

static void i2c_start(struct i2c_adaptor_t *adap)
{
	sdalo(adap);
	scllo(adap);
}

static void i2c_repstart(struct i2c_adaptor_t *adap)
{
	setsda(adap,1);
	sclhi(adap);
	udelay(adap->udelay);

	sdalo(adap);
	scllo(adap);
}

static void i2c_stop(struct i2c_adaptor_t *adap)
{
	sdalo(adap);
	sclhi(adap);
	sdahi(adap);
}

static int i2c_wr_byte(struct i2c_adaptor_t *adap, unsigned char b)
{
	int i, ack;
	
	for (i = 0; i < 8; i++) {
		setsda(adap,b&0x80);
		udelay(adap->udelay);
		if (sclhi(adap)<0) {
			printk("i2c_wr_byte: sclhi(%d) timeout\n", i);
			return(-1);
		}
		scllo(adap);
		b<<=1;
	}
	sdahi(adap);
	if (sclhi(adap)<0) {
		printk("i2c_wr_byte: ack sclhi timeout\n");
	}
	ack = getsda(adap);
	scllo(adap);
	return(ack);
}

static unsigned char i2c_rd_byte(struct i2c_adaptor_t *adap)
{
	int i;
	unsigned char b=0;

	for (i = 0; i < 8; i++) {
		if (sclhi(adap)<0) {
			printk("i2c_rd_byte: sclhi(%d) timeout\n",i);
			return(0);
		}
		b <<= 1;
		b |= (getsda(adap))?1:0;
		scllo(adap);
	}
	return(b);
}

static int i2c_ack(struct i2c_adaptor_t *adap, int neg)
{
	setsda(adap,neg);
	udelay(adap->udelay);
	if (sclhi(adap)<0) {
		printk("i2c_ack: sclhi timeout\n");
		return(0);
	}
	scllo(adap);
	sdahi(adap);
	return(0);	
}

int i2c_transfer(struct i2c_adaptor_t *adap, struct i2c_msg_t *msg, int msg_count)
{
	int i;

	i2c_start(adap);
	for (i=0;i<msg_count;i++) {
		struct i2c_msg_t *pmsg= &msg[i];
		int ack;
		
		if (!(pmsg->flags&I2C_NO_RS)) {
			if (i)
				i2c_repstart(adap);
			ack = i2c_wr_byte(adap, (pmsg->addr<<1)|(pmsg->flags&I2C_RD));
			if (ack) {
				printk("i2c_transfer: ack addr failed\n");
				i2c_stop(adap);
				return(-1);
			}
		}
		if (pmsg->flags&I2C_RD) {
			int j=0, count=pmsg->len;

			while (count--) {
				pmsg->data[j] = i2c_rd_byte(adap);
				ack = i2c_ack(adap, !count);
				if (ack) {
					printk("i2c_transfer: ack rd_byte(%d) failed\n", j);
					i2c_stop(adap);
					return(-1);
				}
				j++;
			}
		} else {
			int j;

			for (j=0; j<pmsg->len; j++) {
				ack = i2c_wr_byte(adap, pmsg->data[j]);
				if (ack) {
					printk("i2c_transfer: ack wr_byte(%d) failed\n", j);
					i2c_stop(adap);
					return(-1);
				}
			}
		}
	}
	i2c_stop(adap);
	return(0);
}

