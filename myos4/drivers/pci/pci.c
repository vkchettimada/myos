#include <stdio.h>
#include <asm/io.h>

typedef struct
{
	unsigned bus, dev_fn;
	unsigned short dev_id;
	unsigned short ven_id;
	unsigned int dev_class;
	unsigned char rev_id;
	unsigned char hdr_type;
	unsigned long base0, base1, base2, base3, base4, base5;
	unsigned char irq_pin, irq;
} pci_dev_t;

static int (*g_pci_read_config_byte)(pci_dev_t *dev,
		unsigned reg, unsigned char *value);
static int (*g_pci_read_config_word)(pci_dev_t *dev,
		unsigned reg, unsigned short *value);
static int (*g_pci_read_config_dword)(pci_dev_t *dev,
		unsigned reg, unsigned long *value);

static int (*g_pci_write_config_byte)(pci_dev_t *dev,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_word)(pci_dev_t *dev,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_dword)(pci_dev_t *dev,
		unsigned reg, unsigned long value);

/*============================================================================
USING TYPE 1 CONFIG
============================================================================*/
#define	CONFIG1_CMD	0xCF8
#define	CONFIG1_DATA	0xCFC
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_byte(pci_dev_t *dev, unsigned reg,
		unsigned char *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	*value = inpb(CONFIG1_DATA + (reg & 3));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_word(pci_dev_t *dev, unsigned reg,
		unsigned short *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	*value = inpw(CONFIG1_DATA + (reg & 2));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_dword (pci_dev_t *dev, unsigned reg,
		unsigned long *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	*value = inpl(CONFIG1_DATA);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_byte(pci_dev_t *dev, unsigned reg,
		unsigned value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	outpb(value, CONFIG1_DATA + (reg & 3));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_word(pci_dev_t *dev, unsigned reg,
		unsigned value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	outpw(value, CONFIG1_DATA + (reg & 2));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_dword(pci_dev_t *dev, unsigned reg,
		unsigned long value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= dev->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= dev->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpl(config_cmd, CONFIG1_CMD);
	outpl(CONFIG1_DATA, value);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_detect(void)
{
	unsigned long temp;

	outpb(0x01, CONFIG1_CMD + 3); /* why? */
	temp = inpl(CONFIG1_CMD);
	outpl(0x80000000L, CONFIG1_CMD);
	if(inpl(CONFIG1_CMD) == 0x80000000L)
	{
		outpl(CONFIG1_CMD, temp);
		g_pci_read_config_byte = type1_read_config_byte;
		g_pci_read_config_word = type1_read_config_word;
		g_pci_read_config_dword = type1_read_config_dword;

		g_pci_write_config_byte = type1_write_config_byte;
		g_pci_write_config_word = type1_write_config_word;
		g_pci_write_config_dword = type1_write_config_dword;
		return 0;
	}
	return -1;
}

/*============================================================================
DETECT PCI DEVICES
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static int pci_scan_dev(pci_dev_t *dev)
{
	unsigned long val;
	
	while(1)
	{
		dev->dev_fn++;
		if(dev->dev_fn >= 256)
		{
			dev->dev_fn = 0;
			dev->bus++;
			if(dev->bus >= 8)
				return 1;
		}
		if((dev->dev_fn & 0x07) == 0)
		{
			if(g_pci_read_config_byte(dev, 0x0E, &dev->hdr_type) != 0)
					return -1;
		}
		/* not a multi-function device */
		else if((dev->hdr_type & 0x80) == 0)
			continue;
		/* 00=PCI_VENDOR_ID */
		if(g_pci_read_config_dword(dev, 0x00, &val) != 0)
			return -1;
		/* some broken boards return 0 if a slot is empty: */
		if(val == 0xFFFFFFFFL || val == 0)
			dev->hdr_type = 0;
		else
			break;
	}

	dev->ven_id = (unsigned)val & 0xFFFF;
	dev->dev_id = (unsigned)val>>16;

	if (g_pci_read_config_dword(dev,0x08, &val) !=0)
		return -1;
	dev->rev_id = val & 0xFF;
	dev->dev_class = val>>8;	
	
	if (g_pci_read_config_dword(dev,0x10, &dev->base0) !=0)
		return -1;
	if (g_pci_read_config_dword(dev,0x14, &dev->base1) !=0)
		return -1;
	
	return 0;
}
/*****************************************************************************
*****************************************************************************/
int pci_init(void)
{
	pci_dev_t dev;

/* detect PCI */
	printk("Checking if type 1 configuration works...\n");
	if(type1_detect())
	{
		printk("No.\n");
		return 1;
	}
/* list all devices */
	dev.bus = -1u;
	dev.dev_fn = 256;
	dev.hdr_type = 0;
	while(pci_scan_dev(&dev) == 0) {
		printk("Bus %d, dev_fn 0x%x, Vendor 0x%x, Device 0x%x, Rev 0x%x, Class 0x%x, Base 0x%x 0x%x\n", dev.bus, dev.dev_fn, dev.ven_id, dev.dev_id, dev.rev_id, dev.dev_class, dev.base0, dev.base1);
	}
	return 0;
}
