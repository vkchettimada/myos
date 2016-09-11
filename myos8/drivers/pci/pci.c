#include <stdio.h>
#include <string.h>
#include <asm/io.h>

#include <driver/pci.h>

/********************************************************************
 *  PCI 64 mandatory registers 0x00-0x3F
 *******************************************************************/
#define PCI_REG_VID                 0x00 /* 16 */
#define PCI_REG_DID                 0x02 /* 16 */
#define PCI_REG_CMD                 0x04 /* 16 */
#define PCI_REG_STAT                0x06 /* 16 */
#define PCI_REG_RID                 0x08 /* 8 */
#define PCI_REG_PROG_INTF           0x09 /* 8 */
#define PCI_REG_SCC                 0x0A /* 8 */
#define PCI_REG_BCC                 0x0B /* 8 */
#define PCI_REG_CACHE_SIZE          0x0C /* 8 */
#define PCI_REG_PMLT                0x0D /* 8 */
#define PCI_REG_HEADER_TYPE         0x0E /* 8 */
#define PCI_REG_BIST                0x0F /* 8 */
#define PCI_REG_BASE_ADDR0          0x10 /* 32 */
#define PCI_REG_BASE_ADDR1          0x14 /* 32 */
#define PCI_REG_BASE_ADDR2          0x18 /* 32 */
#define PCI_REG_BASE_ADDR3          0x1C /* 32 */
#define PCI_REG_BASE_ADDR4          0x20 /* 32 */
#define PCI_REG_BASE_ADDR5          0x24 /* 32 */

#define PCI_REG_SSVID               0x2C /* 16 */
#define PCI_REG_SSID                0x2E /* 16 */

#define PCI_REG_IRQ                 0x3C /* 8 */
#define PCI_REG_IRQ_PIN             0x3D /* 8 */


/********************************************************************
 * Local global definitions
 *******************************************************************/
static struct pci_dev_t pci_dev_list[15];

static int (*g_pci_read_config_byte)(struct pci_dev_t *dev,
		unsigned reg, unsigned char *value);
static int (*g_pci_read_config_word)(struct pci_dev_t *dev,
		unsigned reg, unsigned short *value);
static int (*g_pci_read_config_dword)(struct pci_dev_t *dev,
		unsigned reg, unsigned long *value);

static int (*g_pci_write_config_byte)(struct pci_dev_t *dev,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_word)(struct pci_dev_t *dev,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_dword)(struct pci_dev_t *dev,
		unsigned reg, unsigned long value);

/********************************************************************
 * USING TYPE 1 CONFIG
 *******************************************************************/
#define	CONFIG1_CMD	0xCF8
#define	CONFIG1_DATA	0xCFC

static int type1_read_config_byte(struct pci_dev_t *dev, unsigned reg,
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

static int type1_read_config_word(struct pci_dev_t *dev, unsigned reg,
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

static int type1_read_config_dword (struct pci_dev_t *dev, unsigned reg,
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

static int type1_write_config_byte(struct pci_dev_t *dev, unsigned reg,
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

static int type1_write_config_word(struct pci_dev_t *dev, unsigned reg,
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

static int type1_write_config_dword(struct pci_dev_t *dev, unsigned reg,
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
static void pci_scan_init(struct pci_dev_t *dev)
{
	dev->bus = -1u;
	dev->dev_fn = 256;
	dev->hdr_type = 0;
}

static int pci_scan_dev(struct pci_dev_t *dev)
{
	unsigned long val;
	int base_index;
	
	while(1)
	{
		/* next device function */
		dev->dev_fn++;
		if(dev->dev_fn >= 256)
		{
			/* next bus */
			dev->bus++;
			/* first device function of next bus */
			dev->dev_fn = 0;
			/* stop if 8 buses scanned */
			if(dev->bus >= 8)
				return 1;
		}

		/* if this is not multi-function entry */
		if((dev->dev_fn & 0x07) == 0)
		{
			/* read header type */
			if(g_pci_read_config_byte(dev, PCI_REG_HEADER_TYPE
						, &dev->hdr_type) != 0)
					return -1;
		} else {
			/* check if valid multi-function device
			 * if not continue searching */
			if((dev->hdr_type & 0x80) == 0)
				continue;
		}

		/* read the vendor id and device id */
		if(g_pci_read_config_dword(dev, 0x00, &val) != 0)
			return -1;
		/* check for valid vendor and device id */
		if(val != 0xFFFFFFFFL && val != 0)
			break;
	}

	dev->ven_id = (unsigned)val & 0xFFFF;
	dev->dev_id = (unsigned)val>>16;

	if (g_pci_read_config_byte(dev, PCI_REG_RID, &dev->rev_id) !=0)
		return -1;

	for (base_index = 0; base_index < 6; base_index++) {
		if (g_pci_read_config_dword(dev
					, (PCI_REG_BASE_ADDR0 + (4*base_index) )
					, &dev->base_addr[base_index]) != 0)
			return -1;
	}
	
	if (g_pci_read_config_byte(dev, PCI_REG_IRQ, &dev->irq) !=0)
		return -1;
	if (g_pci_read_config_byte(dev, PCI_REG_IRQ_PIN, &dev->irq_pin) !=0)
		return -1;
	return 0;
}
/*****************************************************************************
*****************************************************************************/
int pci_init(void)
{
	int base_index, count=0;
	struct pci_dev_t dev;

	/* detect PCI */
	printk("Checking if type 1 PCI configuration works...");
	if(type1_detect())
	{
		printk("No.\n");
		return -1;
	}
	printk("Yes.\n");

	/* list all devices */
	printk("Bus  Dev  Fn  Vendor  Device  Rev  irq  irq_pin\n");
	pci_scan_init(&dev);
	while(pci_scan_dev(&dev) == 0) {
		printk("%d  %d  %d  0x%x  0x%x  0x%x  %d  %d  "
				, dev.bus
				, (dev.dev_fn >> 3)
				, (dev.dev_fn & 0x07)
				, dev.ven_id
				, dev.dev_id
				, dev.rev_id
				, dev.irq
				, dev.irq_pin);
		for (base_index = 0; (base_index < 6) && (dev.base_addr[base_index] != 0); base_index++) {
			printk("0x%x  ", dev.base_addr[base_index]);
		}
		printk("\n");

		memcpy(&pci_dev_list[count++], &dev, sizeof(dev));
	}
	return 0;
}

struct pci_dev_t *pci_get_device(unsigned short vendor, unsigned short device)
{
	int i;

	for (i = 0; i < PCI_MAX_DEVICES; i++) {
		if ( pci_dev_list[i].ven_id == vendor
				&& pci_dev_list[i].dev_id == device)
			return(&pci_dev_list[i]);
	}
	return(0);
}

