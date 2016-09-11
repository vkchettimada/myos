/* lpc.c */
#include <stdio.h>
#include <driver/pci.h>
#include <driver/pci_ids.h>

#define LPC_REG_PIRQx	0x60
#define LPC_REG_GEN_CTL	0xD0

static struct pci_dev_t *lpc_pci_dev;

void lpc_enable_pirq(int x)
{
	unsigned char link;

	pci_get_data(lpc_pci_dev, LPC_REG_PIRQx+x-1, 1, &link);
	link &= ~(0x80);
	pci_set_data(lpc_pci_dev, LPC_REG_PIRQx+x-1, 1, &link);
}

void lpc_init(void)
{
	int i;
	unsigned char link;

	lpc_pci_dev = pci_get_device(VENDOR_INTEL, DEVICE_ICH_LPC);
	if (!lpc_pci_dev) {
		printk("lpc: device not found.\n");
		return;
	}
	printk("lpc: vendor= 0x%x device= 0x%x\n"
			, lpc_pci_dev->ven_id
			, lpc_pci_dev->dev_id); 
	for (i=0; i<4; i++) {
		pci_get_data(lpc_pci_dev, LPC_REG_PIRQx+i, 1, &link);
		printk("PIRQ%d-> IRQ%d %sabled.\n", i, link&0x0F
				, (link&0x80)?"dis":"en");
	}
}

