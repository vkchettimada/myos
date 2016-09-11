#ifndef _BT878A_H_
#define _BT878A_H_
#include <asm/io.h>
#include <driver/pci.h>

#define VENDOR_BROOKTREE 0x109e
#define DEVICE_BT878A 0x36e

/* bt878a registers */

#define BT878_DSTATUS		0x000

#define BT878_INT_STAT         0x100
#define BT878_INT_MASK         0x104

#define BT878_INT_ETBF         (1<<23)

#define BT878_INT_RISCS   (0xf<<28)
#define BT878_INT_RISC_EN (1<<27)
#define BT878_INT_RACK    (1<<25)
#define BT878_INT_FIELD   (1<<24)
#define BT878_INT_SCERR   (1<<19)
#define BT878_INT_OCERR   (1<<18)
#define BT878_INT_PABORT  (1<<17)
#define BT878_INT_RIPERR  (1<<16)
#define BT878_INT_PPERR   (1<<15)
#define BT878_INT_FDSR    (1<<14)
#define BT878_INT_FTRGT   (1<<13)
#define BT878_INT_FBUS    (1<<12)
#define BT878_INT_RISCI   (1<<11)
#define BT878_INT_GPINT   (1<<9)
#define BT878_INT_I2CDONE (1<<8)
#define BT878_INT_VPRES   (1<<5)
#define BT878_INT_HLOCK   (1<<4)
#define BT878_INT_OFLOW   (1<<3)
#define BT878_INT_HSYNC   (1<<2)
#define BT878_INT_VSYNC   (1<<1)
#define BT878_INT_FMTCHG  (1<<0)


#define BT878_GPIO_DMA_CTL	0x10C

#define BT878_I2C		0x110
#define BT878_I2C_MODE         (1<<7)
#define BT878_I2C_RATE         (1<<6)
#define BT878_I2C_NOSTOP       (1<<5)
#define BT878_I2C_NOSTART      (1<<4)
#define BT878_I2C_DIV          (0xf<<4)
#define BT878_I2C_SYNC         (1<<3)
#define BT878_I2C_W3B	       (1<<2)
#define BT878_I2C_SCL          (1<<1)
#define BT878_I2C_SDA          (1<<0)


#define BT878_GPIO_OUT_EN	0x118
#define BT878_GPIO_REG_INP	0x11C
#define BT878_GPIO_DATA		0x200

#define btwrite(dat,adr)	writel((dat), (char *) (btdata->mmio + (adr)))
#define btread(adr)		readl(btdata->mmio + (adr))

struct btdata_t {
	char name[15];
	struct pci_dev_t *dev;
	unsigned char *mmio;

	struct i2c_adaptor_t i2c_adap;
	int i2c_state;
};

#endif

