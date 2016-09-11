/* pci.h */
#ifndef _PCI_H_
#define _PCI_H_

/********************************************************************
 * Types
 *******************************************************************/
#define PCI_MAX_DEVICES 15

/********************************************************************
 * Types
 *******************************************************************/
struct pci_dev_t
{
	unsigned bus, dev_fn;
	
	unsigned short dev_id;
	unsigned short ven_id;
	unsigned char rev_id;
	unsigned char hdr_type;
	unsigned long base_addr[6];
	unsigned char irq, irq_pin;
};

/********************************************************************
 * Global definitions
 *******************************************************************/
extern struct pci_dev_t pci_dev_list[PCI_MAX_DEVICES];

/********************************************************************
 * Function prototypes
 *******************************************************************/
int pci_init(void);
struct pci_dev_t *pci_get_device(unsigned short vendor, unsigned short device);

#endif

