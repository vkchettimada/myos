/*****************************************************************************
DJGPP code to detect PCI peripherals,
using either PCI BIOS, type 1 accesses, or type 2 accesses.

Code that uses type 2 accesses is untested.
All write functions are untested.

Chris Giese <geezer@execpc.com>, http://www.execpc.com/~geezer

This code was lifted from Linux bios32.c and pci.c
- Linux bios32.c - Copyright 1994 Drew Eckhardt
- Linux pci.c - Copyright 1993-1995 Drew Eckhardt, Frederic Potter,
	David Mosberger-Tang
Bugs are due to Giese
*****************************************************************************/
#include <sys/farptr.h> /* _farpeekN() */
#include <stdio.h> /* printf() */
#include <dpmi.h> /* __dpmi_NNN() */
#include <go32.h> /* _dos_ds */
#include <pc.h> /* outp(), outpw(), outportl(), inp(), inpw(), inportl() */

#define	inpd(P)		inportl(P)
#define	outpd(P,V)	outportl(P,V)

typedef struct
{
	unsigned bus, dev_fn;
	unsigned char hdr_type;
} state_t;

static int (*g_pci_read_config_byte)(state_t *state,
		unsigned reg, unsigned char *value);
static int (*g_pci_read_config_word)(state_t *state,
		unsigned reg, unsigned short *value);
static int (*g_pci_read_config_dword)(state_t *state,
		unsigned reg, unsigned long *value);

static int (*g_pci_write_config_byte)(state_t *state,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_word)(state_t *state,
		unsigned reg, unsigned value);
static int (*g_pci_write_config_dword)(state_t *state,
		unsigned reg, unsigned long value);
/*============================================================================
USING PCI BIOS
============================================================================*/
#define PCIBIOS_READ_CONFIG_BYTE	0xB108
#define PCIBIOS_READ_CONFIG_WORD	0xB109
#define PCIBIOS_READ_CONFIG_DWORD	0xB10A
#define PCIBIOS_WRITE_CONFIG_BYTE	0xB10B
#define PCIBIOS_WRITE_CONFIG_WORD	0xB10C
#define PCIBIOS_WRITE_CONFIG_DWORD	0xB10D

static unsigned char g_bios32_far_entry[6];
/*****************************************************************************
*****************************************************************************/
static int bios_read_config_byte(state_t *state, unsigned reg,
		unsigned char *value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	__asm__("lcall *(%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah,%%ah\n"
		"1:"
		: "=c"(*value),
		  "=a"(ret)
		: "1"(PCIBIOS_READ_CONFIG_BYTE),
		  "b"(bx),
		  "D"(reg),
		  "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_read_config_word(state_t *state, unsigned reg,
		unsigned short *value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	__asm__("lcall *(%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah,%%ah\n"
		"1:"
		: "=c"(*value),
		  "=a"(ret)
		: "1"(PCIBIOS_READ_CONFIG_WORD),
		  "b"(bx),
		  "D"(reg),
		  "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_read_config_dword(state_t *state, unsigned reg,
		unsigned long *value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	__asm__("lcall *(%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah,%%ah\n"
		"1:"
		: "=c"(*value),
		  "=a"(ret)
		: "1"(PCIBIOS_READ_CONFIG_DWORD),
		  "b"(bx),
		  "D"(reg),
		  "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_write_config_byte(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	 __asm__("lcall *(%%esi)\n\t"
		 "jc 1f\n\t"
		 "xor %%ah,%%ah\n"
		 "1:"
		 : "=a"(ret)
		 : "0"(PCIBIOS_WRITE_CONFIG_DWORD),
		   "c"(value),
		   "b"(bx),
		   "D"(reg),
		   "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_write_config_word(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	 __asm__("lcall *(%%esi)\n\t"
		 "jc 1f\n\t"
		 "xor %%ah,%%ah\n"
		 "1:"
		 : "=a"(ret)
		 : "0"(PCIBIOS_WRITE_CONFIG_WORD),
		   "c"(value),
		   "b"(bx),
		   "D"(reg),
		   "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_write_config_dword(state_t *state, unsigned reg,
		unsigned long value)
{
	unsigned bx, ret;

	bx = state->bus;
	bx <<= 8;
	bx |= state->dev_fn;
	 __asm__("lcall *(%%esi)\n\t"
		 "jc 1f\n\t"
		 "xor %%ah,%%ah\n"
		 "1:"
		 : "=a"(ret)
		 : "0"(PCIBIOS_WRITE_CONFIG_BYTE),
		   "c"(value),
		   "b"(bx),
		   "D"(reg),
		   "S"(&g_bios32_far_entry));
	ret >>= 8;
	return ret & 0xFF;
}
/*****************************************************************************
*****************************************************************************/
static int bios_detect(void)
{
	static unsigned char descriptor[8] =
	{
		0xFF, 0xFF,/* limit b15:b0 */
		0x00, 0x00, 0x00, /* base b23:b0 */
		0xFA,	/* present, ring 3, code, non-conforming, readable */
		0xCF,	/* page-granular and 32-bit; limit b19:b16 */
		0	/* base b31:b24 */
	};
/**/
	unsigned adr, temp, len, sel;
	unsigned char sum, err;

/* scan for BIOS32 */
	for(adr = 0xE0000; adr < 0xFFFF0; adr += 16)
/* signature */
	{
		temp = _farpeekl(_dos_ds, adr);
		if(temp != 0x5F32335F)	/* "_32_" */
			continue;
/* structure len */
		temp = _farpeekb(_dos_ds, adr + 9) * 16;
		if(temp == 0)
			continue;
/* checksum of the structure must be 0 */
		for(sum = 0; temp != 0; temp--)
			sum += _farpeekb(_dos_ds, adr + temp - 1);
		if(sum != 0)
			continue;
/* revision */
		temp = _farpeekb(_dos_ds, adr + 8);
		if(temp != 0)
		{
			printf("Unsupported BIOS32 revision %d\n", temp);
			continue;
		}
/* found it, get BIOS32 entry point */
		*(unsigned long *)(g_bios32_far_entry + 0) =
			_farpeekl(_dos_ds, adr + 4);
/* linear (base address == 0) code segment/descriptor/selector */
		sel = __dpmi_allocate_ldt_descriptors(1);
		if(__dpmi_set_descriptor(sel, descriptor) < 0)
		{
			printf("Could not install linear code "
				"segment descriptor\n");
			return -1;
		}
		*(unsigned short *)(g_bios32_far_entry + 4) = sel;
/* now get entry point of 32-bit PCI BIOS */
		__asm__("lcall *(%%edi)"
			: "=a"(err),	/* AL out=status */
			  "=b"(adr),	/* EBX out=code segment base adr */
			  "=c"(len),	/* ECX out=code segment size */
			  "=d"(temp)	/* EDX out=entry pt offset in code */
			: "0"(0x49435024),/* EAX in=service="$PCI" */
			  "1"(0),	/* EBX in=0=get service entry pt */
			  "D"(&g_bios32_far_entry));
		if(err == 0x80)
		{
			printf("BIOS32 found, but no PCI BIOS (?)\n");
			return -1;
		}
		if(err != 0)
		{
			printf("BIOS32 call to locate PCI BIOS returned "
				"0x%X (?)\n", err);
			return -1;
		}
/* got it */
		*(unsigned long *)(g_bios32_far_entry + 0) = adr + temp;
		g_pci_read_config_dword = bios_read_config_dword;
		g_pci_read_config_word = bios_read_config_word;
		g_pci_read_config_byte = bios_read_config_byte;

		g_pci_write_config_dword = bios_write_config_dword;
		g_pci_write_config_word = bios_write_config_word;
		g_pci_write_config_byte = bios_write_config_byte;
		return 0;
	}
/* not detected */
	return -1;
}
/*============================================================================
USING TYPE 1 CONFIG
============================================================================*/
#define	CONFIG1_ADR	0xCF8
#define	CONFIG1_DATA	0xCFC
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_byte(state_t *state, unsigned reg,
		unsigned char *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	*value = inp(CONFIG1_DATA + (reg & 3));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_word(state_t *state, unsigned reg,
		unsigned short *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	*value = inpw(CONFIG1_DATA + (reg & 2));
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_read_config_dword (state_t *state, unsigned reg,
		unsigned long *value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	*value = inpd(CONFIG1_DATA);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_byte(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	outp(CONFIG1_DATA + (reg & 3), value);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_word(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	outpw(CONFIG1_DATA + (reg & 2), value);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_write_config_dword(state_t *state, unsigned reg,
		unsigned long value)
{
	unsigned long config_cmd;

	config_cmd = 0x80;		/* b31    = enable configuration space mapping */
	config_cmd <<= 8;
	config_cmd |= state->bus;	/* b23:16 = bus number */
	config_cmd <<= 8;
	config_cmd |= state->dev_fn;	/* b15:8  = dev (b15:11) fn (b10:8) */
	config_cmd <<= 8;
	config_cmd |= (reg & ~3);	/* b7:2   = register number */
	outpd(CONFIG1_ADR, config_cmd);
	outpd(CONFIG1_DATA, value);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type1_detect(void)
{
	unsigned long temp;

	outp(CONFIG1_ADR + 3, 0x01); /* why? */
	temp = inpd(CONFIG1_ADR);
	outpd(CONFIG1_ADR, 0x80000000L);
	if(inpd(CONFIG1_ADR) == 0x80000000L)
	{
		outpd(CONFIG1_ADR, temp);
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
USING TYPE 2 CONFIG
"this configuration mechanism is deprecated as of PCI version 2.1;
 only mechanism 1 should be used for new systems"
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static int type2_read_config_byte(state_t *state, unsigned reg,
		unsigned char *value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	*value = inp(x);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_read_config_word(state_t *state, unsigned reg,
		unsigned short *value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	*value = inpw(x);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_read_config_dword(state_t *state, unsigned reg,
		unsigned long *value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	*value = inpd(x);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_write_config_byte(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	outp(x, value);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_write_config_word(state_t *state, unsigned reg,
		unsigned value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	outpw(x, value);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_write_config_dword(state_t *state, unsigned reg,
		unsigned long value)
{
	unsigned x;

	if((state->dev_fn & 0x80) != 0)
		return -1;
	x = state->dev_fn & 7;		/* function */
	x <<= 1;
	x |= 0xF0;
	outp(0xCF8, x);
	outp(0xCFA, state->bus);
	x = state->dev_fn & 0xF8;	/* device */
	x <<= 5;
	x |= reg;
	x |= 0xC000;
	outpd(x, value);
	outp(0xCF8, 0);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int type2_detect(void)
{
	outp(0xCFB, 0x00);
	outp(0xCF8, 0x00);
	outp(0xCFA, 0x00);
	if(inp(0xCF8) == 0x00 && inp(0xCFB) == 0x00)
	{
		g_pci_read_config_byte = type2_read_config_byte;
		g_pci_read_config_word = type2_read_config_word;
		g_pci_read_config_dword = type2_read_config_dword;

		g_pci_write_config_byte = type2_write_config_byte;
		g_pci_write_config_word = type2_write_config_word;
		g_pci_write_config_dword = type2_write_config_dword;
		return 0;
	}
	return -1;
}
/*============================================================================
DETECT PCI DEVICES
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static int pci_scan_vendor(state_t *state, unsigned *vendor, unsigned *dev)
{
	unsigned long vdev = 0;

	while(1)
	{
		state->dev_fn++;
		if(state->dev_fn >= 256)
		{
			state->dev_fn = 0;
			state->bus++;
			if(state->bus >= 8)
				return 1;
			printf("Scanning PCI bus %u...\n", state->bus);
		}
		if((state->dev_fn & 0x07) == 0)
		{
			if(g_pci_read_config_byte(state,
/* 0E=PCI_HEADER_TYPE */
				0x0E, &state->hdr_type) != 0)
					return -1;
		}
/* not a multi-function device */
		else if((state->hdr_type & 0x80) == 0)
			continue;
/* 00=PCI_VENDOR_ID */
		if(g_pci_read_config_dword(state, 0x00, &vdev) != 0)
			return -1;
/* some broken boards return 0 if a slot is empty: */
		if(vdev == 0xFFFFFFFFL || vdev == 0)
			state->hdr_type = 0;
		else
			break;
	}
	*vendor = (unsigned)vdev & 0xFFFF;
	vdev >>= 16;
	*dev = (unsigned)vdev;
	printf("dev_fn=%3u, Vendor 0x%04X, Device 0x%04X\n",
		state->dev_fn, *vendor, *dev);
	return 0;
}
/*****************************************************************************
'class' is a C++ reserved word
*****************************************************************************/
static int pci_scan_class(state_t *state, unsigned *_class, unsigned *sub)
{
	unsigned long vdev = 0;
	unsigned char c;

	while(1)
	{
		state->dev_fn++;
		if(state->dev_fn >= 256)
		{
			state->dev_fn = 0;
			state->bus++;
			if(state->bus >= 8)
				return -1;
			printf("Scanning PCI bus %u...\n", state->bus);
		}
		if((state->dev_fn & 0x07) == 0)
		{
			if(g_pci_read_config_byte(state,
/* 0E=PCI_HEADER_TYPE */
				0x0E, &state->hdr_type) != 0)
					return -1;
		}
/* not a multi-function device */
		else if((state->hdr_type & 0x80) == 0)
			continue;
/* 00=PCI_VENDOR_ID */
		if(g_pci_read_config_dword(state, 0x00, &vdev) != 0)
			return -1;
/* some broken boards return 0 if a slot is empty: */
		if(vdev == 0xFFFFFFFFL || vdev == 0)
			state->hdr_type = 0;
		else
			break;
	}
	if(g_pci_read_config_byte(state, 0x0A, &c) != 0)
		return -1;
	*sub = c;
	if(g_pci_read_config_byte(state, 0x0B, &c) != 0)
		return -1;
	*_class = c;
	printf("dev_fn=%3u, Class %u.%u\n", state->dev_fn, *_class, *sub);
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int detect_pci(void)
{
	printf("Checking for PCI BIOS...\n");
	if(bios_detect() == 0)
		return 0;
	printf("No PCI BIOS; checking if type 1 configuration works...\n");
	if(type1_detect() == 0)
		return 0;
	printf("no; checking if type 2 configuration works...\n");
	if(type2_detect() == 0)
		return 0;
	return -1;
}
/*****************************************************************************
*****************************************************************************/
int main(void)
{
	unsigned vendor, dev, _class, sub;
	state_t state;

/* detect PCI */
	if(detect_pci())
	{
		printf("No PCI BIOS/devices detected\n");
		return 1;
	}
/* list all devices */
	state.bus = -1u;
	state.dev_fn = 256;
	state.hdr_type = 0;
	while(pci_scan_vendor(&state, &vendor, &dev) == 0)
		/* nothing */;
/* find USB controller */
	printf("\nChecking for USB controller...\n");
	state.bus = -1u;
	state.dev_fn = 256;
	state.hdr_type = 0;
	while(pci_scan_class(&state, &_class, &sub) == 0)
	{
		if(_class == 12 && /* serial bus controller */
			sub == 3) /* USB */
		{
			printf("Found USB controller\n");
			return 0;
		}
	}
	printf("No USB controller found\n");
	return 0;
}
