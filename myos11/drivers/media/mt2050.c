#include <stdio.h>
#include <string.h>
#include <driver/i2c.h>

#define I2C_ADDR_MT2050 0x60 /* 0x60 - 0x6f */
#define MT2050_USE_RADIO_ANTENNA 0

static unsigned char mt2050_rd(struct i2c_adaptor_t *adap, unsigned char reg)
{
	unsigned char data;
	struct i2c_msg_t msg[2] = {
		{0, I2C_ADDR_MT2050, 1, &reg},
		{I2C_RD, I2C_ADDR_MT2050, 1, &data}
	};
	int res;

	res=i2c_transfer(adap, msg, 2);
	if (res) {
		printk("mt2050_rd: failed\n");
		return(0);
	}
	
	return(data);
}

static int mt2050_wr(struct i2c_adaptor_t *adap, unsigned char reg, unsigned char data)
{
	unsigned char buf[2]= {
		reg,
		data
	};
	struct i2c_msg_t msg[1] = {
		{0, I2C_ADDR_MT2050, 2, buf}
	};
	int res;

	res=i2c_transfer(adap, msg, 1);
	if (res) {
		printk("mt2050_wr: failed\n");
		return(0);
	}
	
	return(res);
}

static int mt2050_set_if_freq(struct i2c_adaptor_t *adap,unsigned int freq, unsigned int if2)
{
	unsigned int if1=1218*1000*1000;
	unsigned int f_lo1, f_lo2, lo1, lo2, f_lo1_modulo, f_lo2_modulo, num1, num2, div1a, div1b, div2a, div2b;
	unsigned char buf[6];
	struct i2c_msg_t msg[1] = {
		{0, I2C_ADDR_MT2050, 6, buf}
	};
	int res;
	
	f_lo1=freq+if1;
	f_lo1=(f_lo1/1000000)*1000000;
	
	f_lo2=f_lo1-freq-if2;
	f_lo2=(f_lo2/50000)*50000;
	
	lo1=f_lo1/4000000;
	lo2=f_lo2/4000000;
	
	f_lo1_modulo= f_lo1-(lo1*4000000);
	f_lo2_modulo= f_lo2-(lo2*4000000);
	
	num1=4*f_lo1_modulo/4000000;
	num2=4095*(f_lo2_modulo/1000)/4000;
	
	// todo spurchecks
	
	div1a=(lo1/12)-1;
	div1b=lo1-(div1a+1)*12;
	
	div2a=(lo2/8)-1;
	div2b=lo2-(div2a+1)*8;
	
	buf[0]=1;
	buf[1]= 4*div1b + num1;
	if(freq<277*1000*1000) buf[1] = buf[1]|0x80;
	
	buf[2]=div1a;
	buf[3]=32*div2b + num2/256;
	buf[4]=num2-(num2/256)*256;
	buf[5]=div2a;
	if(num2!=0) buf[5]=buf[5]|0x40;
	
	printk("mt2050: buf= %x %x %x %x %x %x\n",buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	
	res=i2c_transfer(adap, msg, 1);
        if (res)
                printk("mt2050_set_if_freq failed\n");

	return(res);
}

int mt2050_set_radio_freq(struct i2c_adaptor_t *adap, unsigned int freq)
{
	int res;
	
	res = mt2050_set_if_freq(adap, freq, 33300*1000);
	return(res);
}

int mt2050_init(struct i2c_adaptor_t *adap)
{
	unsigned char buf[21];
	struct i2c_msg_t msg[2] = {
		{0, I2C_ADDR_MT2050, 1, buf},
		{I2C_RD, I2C_ADDR_MT2050, 20, &buf[1]}
	};
	int res, i;
	
	memset(buf,0,sizeof(buf));
	res = i2c_transfer(adap, msg, 2);
	if (res) {
		printk("mt2050_init: i2c_transfer failed\n");
		return(0);
	}
	
	printk("mt2050_init: buf= ");
	for (i=0;i<21;i++)
		printk("0x%x ",buf[i]);
	printk("\n");

	mt2050_wr(adap,0x01, 0x2F);
	mt2050_wr(adap,0x02, 0x25);
	mt2050_wr(adap,0x03, 0xC1);
	mt2050_wr(adap,0x04, 0x00);
    	mt2050_wr(adap,0x05, 0x63);
    	mt2050_wr(adap,0x06, (MT2050_USE_RADIO_ANTENNA)?0x10:0x11);
    	mt2050_wr(adap,0x0a, 0x85);
    	mt2050_wr(adap,0x0d, 0x28);
    	mt2050_wr(adap,0x0f, 0x0F);
	mt2050_wr(adap,0x10, 0x24);

	buf[0]=mt2050_rd(adap,0x0d);
	printk("mt2050: sro is %x\n",buf[0]);
	
	return 0;
}

int mt2050_deinit(struct i2c_adaptor_t *adap)
{
	unsigned char buf[2];
	struct i2c_msg_t msg[1] = {
		{0, I2C_ADDR_MT2050, 2, buf}
	};
	int res;
	
	buf[0]=6;
	buf[1]=0x00;
	res=i2c_transfer(adap, msg, 1); //  power off, bit4=0
        if (res)
                printk("mt2050_deinit failed\n");

	return(res);
}

