#include <stdio.h>
#include <asm/delay.h>
#include <driver/i2c.h>
#include <driver/media.h>

#define I2C_ADDR_MSP34X0G 0x40

#define I2C_MSP34X0G_DEM 0x10
#define I2C_MSP34X0G_DSP 0x12

static int msp34x0g_read(struct i2c_adaptor_t *adap,
	       	unsigned char subaddr,
		unsigned short reg,
		unsigned short *value)
{
	unsigned char wbuf[3];
	unsigned char rbuf[2];
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
	char reset[3]={ 0x00, 0x80, 0x00};
	msp34x0g_write(adap,reset,3);

	char normal[3]={ 0x00, 0x00, 0x00};
	msp34x0g_write(adap,normal,3);

	char speaker[5] = { 0x12, 0x00, 0x00, 0x73, 0x00 };
	msp34x0g_write(adap,speaker,5);

	return(msp34x0g_version(adap));
}

int msp34x0g_detect(struct i2c_adaptor_t *adap)
{
	char carrier_mute[5] = { 0x10, 0x00, 0x24, 0x07, 0xFF };
	msp34x0g_write(adap,carrier_mute,5);

	char modus[5] = { 0x10, 0x00, 0x30, 0x20, 0x01 };
	msp34x0g_write(adap,modus,5);

	char spk_source[5] = { 0x12, 0x00, 0x08, 0x01, 0x20 };
	msp34x0g_write(adap,spk_source,5);

	char spk_prescale[5] = { 0x12, 0x00, 0x0E, 0x24, 0x02 };
	msp34x0g_write(adap,spk_prescale,5);

	char select[5] = { 0x10, 0x00, 0x20, 0x00, 0x01 };
	msp34x0g_write(adap,select,5);
	
	int count=0;
	short ret=0;
	do {
		count++;
		msp34x0g_read(adap,I2C_MSP34X0G_DEM+1,0x007E,&ret);
		printk("msp34x0g: Detecting standard returned 0x%x\n",ret);
		udelay(100000);
	} while (count<10 && ret>0x07FF);

	static char a2_thres[5] = { 0x10, 0x00, 0x22, 0x01, 0x90 };
	msp34x0g_write(adap,a2_thres,5);
	
	return(0);
}

