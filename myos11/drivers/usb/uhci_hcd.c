/* uhci_hcd.c */
#include <stdio.h>
#include <driver/pci.h>
#include <driver/pci_ids.h>

struct uhci_hcd_t {
	struct pci_dev_t *dev;
	unsigned int ioport;
	unsigned int ioport_len;
};


void uhci_hcd_init(void)
{
	static struct uhci_hcd_t hcd_inst;
	struct uhci_hcd_t *hcd = &hcd_inst;

	hcd->dev = pci_get_device(VENDOR_INTEL, DEVICE_ICH_UHCI_HCD);
	if (!hcd->dev) {
		printk("uhci_hcd: device not found.\n");
		return;
	}

	printk("uhci_hcd: vendor= 0x%x device= 0x%x\n"
			, ac97_inst->dev->ven_id 
			, ac97_inst->dev->dev_id); 
}

