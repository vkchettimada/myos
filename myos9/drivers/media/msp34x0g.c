#include <stdio.h>
#include <asm/delay.h>
#include <driver/i2c.h>
#include <driver/media.h>

#define I2C_ADDR_MSP34X0G 0x40

#define I2C_MSP34X0G_DEM 0x10
#define I2C_MSP34X0G_DSP 0x12

int autosnd=0x1;
int detect=1;
int matrix=0x20;
int debug=0;

static int msp34x0g_read(struct i2c_adaptor_t *adap,
	       	unsigned char subaddr,
		unsigned short reg,
		unsigned short *value)
{
	static unsigned char wbuf[3];
	static unsigned char rbuf[2];
	struct i2c_msg_t msg[2]= {
		{0, I2C_ADDR_MSP34X0G, 3, wbuf},
		{I2C_RD, I2C_ADDR_MSP34X0G, 2, rbuf},
	};
	int res;
	
	wbuf[0]=subaddr;
	wbuf[1]=(reg&0xFF00)>>8;
	wbuf[2]=reg&0xFF;
	
	res = i2c_transfer(adap, msg, 2);
	if (res) {
		printk("msp34x0g: read failed\n");
		return(res);
	}
	*value=(rbuf[0]<<8)|rbuf[1];
	return(res);
}

static int msp34x0g_write(struct i2c_adaptor_t *adap,
	       	unsigned char *buf,
		unsigned int len)
{
	struct i2c_msg_t msg[1]= {
		{0, I2C_ADDR_MSP34X0G, len, buf},
	};
	int res;
	
	res = i2c_transfer(adap, msg, 1);
	if (res) {
		printk("msp34x0g: write failed\n");
		return(res);
	}
	return(res);
}

static int msp34x0g_version(struct i2c_adaptor_t *adap)
{
	unsigned short value=0;
	
	if (msp34x0g_read(adap, I2C_MSP34X0G_DSP+1, 0x001E, &value)==0)
	{
		printk("msp34x0g: version= 0x%x revision= 0x%x\n",(value>>8),value&0xff);
		return (!(value>0)); 
	}
	return -1;
}

int msp34x0g_reset(struct i2c_adaptor_t *adap)
{
	static char reset[3]={ 0x00, 0x80, 0x00};
	msp34x0g_write(adap,reset,3);

	static char normal[3]={ 0x00, 0x00, 0x00};
	msp34x0g_write(adap,normal,3);

	static char speaker[5] = { 0x12, 0x00, 0x00, 0x73, 0x00 };
	msp34x0g_write(adap,speaker,5);

	return(msp34x0g_version(adap));
}

int msp34x0g_detect(struct i2c_adaptor_t *adap)
{
	static char carrier_mute[5] = { 0x10, 0x00, 0x24, 0x07, 0xFF };
	msp34x0g_write(adap,carrier_mute,5);

	static char a2_thres[5] = { 0x10, 0x00, 0x22, 0x01, 0x90 };
	msp34x0g_write(adap,a2_thres,5);
	
	static char modus[5] = { 0x10, 0x00, 0x30, 0x20, 0x01 };
	modus[4]=autosnd;
	msp34x0g_write(adap,modus,5);

	static char spk_source[5] = { 0x12, 0x00, 0x08, 0x01, 0x20 };
	spk_source[4]=matrix;
	msp34x0g_write(adap,spk_source,5);

	static char spk_prescale[5] = { 0x12, 0x00, 0x0E, 0x24, 0x02 };
	msp34x0g_write(adap,spk_prescale,5);

	static char select[5] = { 0x10, 0x00, 0x20, 0x00, 0x40 };
	
	if (detect) {
		select[4]=0x01;
	}
	msp34x0g_write(adap,select,5);
	
	int count=0;
	short ret=0;
	do {
		count++;
		msp34x0g_read(adap,I2C_MSP34X0G_DEM+1,0x007E,&ret);
		printk("msp34x0g: Detecting standard returned 0x%x\n",ret);
		udelay(100000);
	} while (count<10 && ret>0x07FF);

	return(0);
}
/*
static int msp34x0g_command(struct i2c_adaptor_t *adap, unsigned int cmd, void *arg)
{
	switch(cmd)
	{
		case VIDIOCSFREQ:
			msp34x0g_detect(client);
			break;
		case V4L2_CID_AUDIO_VOLUME:
			{
				if (debug) printk(KERN_DEBUG"msp34x0g_command: volume called\n");
				int volume=*(int*)arg;
				if (volume&0x80000000) {
					static char speaker[5] = { 0x12, 0x00, 0x00, 0x00, 0x00 };
					speaker[3]=(volume>>8)&0xff;
					speaker[4]=volume&0xff;
					msp34x0g_write(adap,speaker,5);
				} else {
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0000,arg);					
				}				
			}
			break;
		case MSP34X0G_IOCTL_TONE_MODE:
			{
				static char mode[5]={ I2C_MSP34X0G_DSP, 0x00, 0x20, 0x00, 0x00};
				mode[3]=(*(int*)arg)&0xff;
				msp34x0g_write(adap,mode,5);
			}
			break;
		case MSP34X0G_IOCTL_EQ:
			{
				if (debug) printk(KERN_DEBUG"msp34x0g_command: eq called\n");
				struct msp34x0g_eq *eq=(struct msp34x0g_eq*)arg;
				if (eq->dir) {
					static char band[5]={ I2C_MSP34X0G_DSP, 0x00, 0x00, 0x00, 0x00};
					band[2]=0x21;
					band[3]=eq->band1>>8;
					band[4]=eq->band1&0xff;
					msp34x0g_write(adap,band,5);
					
					band[2]=0x22;
					band[3]=eq->band2>>8;
					band[4]=eq->band2&0xff;
					msp34x0g_write(adap,band,5);
					
					band[2]=0x23;
					band[3]=eq->band3>>8;
					band[4]=eq->band3&0xff;
					msp34x0g_write(adap,band,5);
					
					band[2]=0x24;
					band[3]=eq->band4>>8;
					band[4]=eq->band4&0xff;
					msp34x0g_write(adap,band,5);
					
					band[2]=0x25;
					band[3]=eq->band5>>8;
					band[4]=eq->band5&0xff;
					msp34x0g_write(adap,band,5);
					
				} else {
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0021,&eq->band1);
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0022,&eq->band2);
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0023,&eq->band3);
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0024,&eq->band4);
				  msp34x0g_read(client,I2C_MSP34X0G_DSP+1,0x0025,&eq->band5);
				}
			}
			break;
		default:
			break;
	}
	return(0);
}
*/

