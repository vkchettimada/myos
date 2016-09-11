/* bt878a.c */
#include <stdio.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/i2c.h>
#include <driver/video.h>
#include "bt878a.h"

static struct btdata_t _btdata;

static void bt878a_bit_setscl(void *data, int state)
{
	struct btdata_t *btdata=(struct btdata_t*) data;

	if (state)
		btdata->i2c_state |= 0x2;
	else
		btdata->i2c_state &= ~0x2;
	btwrite(btdata->i2c_state, BT878_I2C);
	btread(BT878_I2C);
}

static void bt878a_bit_setsda(void *data, int state)
{
	struct btdata_t *btdata=(struct btdata_t*) data;

	if (state)
		btdata->i2c_state |= 0x1;
	else
		btdata->i2c_state &= ~0x1;
	btwrite(btdata->i2c_state, BT878_I2C);
	btread(BT878_I2C);
}

static int bt878a_bit_getscl(void *data)
{
	struct btdata_t *btdata=(struct btdata_t*) data;
	int state;

	state=btread(BT878_I2C) & 0x2 ? 1 : 0;
	return(state);
	
}

static int bt878a_bit_getsda(void *data)
{
	struct btdata_t *btdata=(struct btdata_t*) data;
	int state;

	state=btread(BT878_I2C) & 0x1 ? 1 : 0;
	return(state);
}

static struct i2c_adaptor_t bt878a_i2c_adaptor = {
	.setscl = bt878a_bit_setscl,
	.setsda = bt878a_bit_setsda,
	.getscl = bt878a_bit_getscl,
	.getsda = bt878a_bit_getsda,
	.udelay = 5,
	.timeout = 200,
	.data = (void*) &_btdata,
};


void bt878a_init(void)
{
	struct btdata_t *btdata = (struct btdata_t*) bt878a_i2c_adaptor.data;
	
	btdata->dev=pci_get_device( VENDOR_BROOKTREE, DEVICE_BT878A );
	if (!btdata->dev) {
		printk("bt878a: device not found.\n");
		return;
	}

	btdata->mmio = (unsigned char*) (btdata->dev->base_addr[0] & (~0xf));
	
	printk("bt878a: vendor= 0x%x device= 0x%x mmio= 0x%x\n"
			, btdata->dev->ven_id 
			, btdata->dev->dev_id 
			, btdata->mmio);

	/* read the current value of BT878_I2C register */
	btdata->i2c_state = btread(BT878_I2C);
	printk("bt878a: i2c_state = 0x%x\n", btdata->i2c_state);

	/* initialise mt2050 */
	mt2050_init(&bt878a_i2c_adaptor);
	
	/* test msp34x0g */
	{
		#define I2C_ADDR_MSP34X0G 0x40
		unsigned char data[5] = {
			0x13, 0x00, 0x1F,
		};
		struct i2c_msg_t msg[2] = {
			{0, I2C_ADDR_MSP34X0G, 3, data},
			{I2C_RD, I2C_ADDR_MSP34X0G, 2, &data[3]}
		};
		int res;

		res=i2c_transfer(&bt878a_i2c_adaptor, msg, 2);
		printk("bt878a: msp34x0g i2c res=%d, prod= 0x%x, rom= 0x%x\n"
				, res, data[3], data[4]);
	}
}

