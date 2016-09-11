/* bt878a.c */
#include <stdio.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/i2c.h>
#include <driver/media.h>
#include "bt878a.h"

#define BT878A_GPIOMASK 0x23000f
#define BT878A_RADIOMUX 0x220001
#define BT878A_MUTEMUX  0x22000b

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

static struct i2c_adaptor_t *g_adap=0;

struct i2c_adaptor_t *bt878a_get_adaptor(void)
{
	return(g_adap);
}

void bt878a_init(void)
{
	struct i2c_adaptor_t *adap = &bt878a_i2c_adaptor;
	struct btdata_t *btdata = (struct btdata_t*) adap->data;
	unsigned int int_stat;
	
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

	/* clear int mask */
	btwrite(0x0, BT878_INT_MASK);
	/* clear int status */
	int_stat = btread(BT878_INT_MASK);
	btwrite(int_stat, BT878_INT_STAT);
	printk("bt878a_init: int_stat = 0x%x\n", int_stat);
	/* TBD */
	btwrite(0x00, BT878_GPIO_REG_INP);
	/* set the GPIO mask */
	btwrite(BT878A_GPIOMASK, BT878_GPIO_OUT_EN);
	
	/* read the current value of BT878_I2C register */
	btdata->i2c_state = btread(BT878_I2C);
	printk("bt878a: i2c_state = 0x%x\n", btdata->i2c_state);

	/* initialise mt2050 */
	mt2050_init(adap);
	mt2050_set_radio_freq(adap, 91*1000*1000);

	/* initialise tda9887t */
	tda9887t_configure(adap, TDA9887T_RADIO_STD_FM);
	
	/* initialise msp34x0g */
	msp34x0g_reset(adap);
	msp34x0g_detect(adap);

	/* set mux to radio */
	btwrite(BT878A_RADIOMUX, BT878_GPIO_DATA);
	
	printk("bt878a: gpio_data=0x%x\n", btread(BT878_GPIO_DATA));

	g_adap = adap;
}

