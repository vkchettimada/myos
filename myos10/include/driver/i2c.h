/* i2c.h */
#ifndef _I2C_H_
#define _I2C_H_

#include <config.h>

struct i2c_adaptor_t
{
	char name[CONFIG_NAME_LEN];
	void (*setscl)(void *data, int state);
	void (*setsda)(void *data, int state);
	int (*getscl)(void *data);
	int (*getsda)(void *data);
	unsigned int udelay;
	unsigned int timeout;
	void *data;
};

struct i2c_msg_t
{
	unsigned char flags;
#define I2C_RD    0x01
#define I2C_NO_RS 0x02
	unsigned char addr;
	int len;
	unsigned char *data;
};

int i2c_transfer(struct i2c_adaptor_t *adap, struct i2c_msg_t *msg, int msg_count);

#endif

