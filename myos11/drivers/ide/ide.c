/* ide.c */
#include <stdio.h>
#include <string.h>
#include <asm/io.h>
#include <asm/delay.h>

#define IDE_PRIMARY_BASE	0x1F0

#define IDE_DATA		0x00
#define IDE_FEATURE		0x01
#define IDE_ERROR		0x01
#define IDE_NSECTOR		0x02
#define IDE_SECTOR		0x03
#define IDE_CYL_LSB		0x04
#define IDE_CYL_MSB		0x05
#define IDE_SELECT		0x06
#define IDE_SELECT_DEFAULT		0xA0
#define IDE_SELECT_HEAD(x)		((x) & 0x0F)
#define IDE_SELECT_MASTER		0x00
#define IDE_SELECT_SLAVE		0x10
#define IDE_SELECT_LBA			0x40
#define IDE_SELECT_CHS			0x00
#define IDE_CMD			0x07
#define IDE_CMD_PACKET			0xA0
#define IDE_STATUS		0x07
#define IDE_STATUS_BUSY			0x80
#define IDE_STATUS_DRQ			0x08

#define ide_inpb(offset)	inpb((ide_base)+(offset))
#define ide_inpsw(offset,x,c)	inpsw(((ide_base)+(offset)),(x),(c))
#define ide_outpb(x,offset)	outpb((x),((ide_base)+(offset)))
#define ide_outpsw(x,c,offset)	outpsw((x),(c),((ide_base)+(offset)))

#define ATAPI_REQUEST_SENSE	0x03
#define ATAPI_READ_CAPACITY	0x25
#define ATAPI_PLAY_AUDIO_10	0x45

static int be2le(unsigned char *byte,int n)
{
	int i,x=0;

	for (i=0;i<n;i++) {
		x<<=8;
		x|=*(byte++);
	}
	return(x);
}

static int ide_not_busy(unsigned short ide_base)
{
	unsigned char status;

	status = ide_inpb(IDE_STATUS);
	//printk("ide_status= 0x%x\n", status);
	return(!(status&IDE_STATUS_BUSY));
}

static int ide_wait(int (*done)(unsigned short)
		, unsigned short ide_base
		, int timeout)
{
	while (timeout--) {
		if (done(ide_base))
			return(0);
		udelay(1000);
	}
	printk("ide_wait: timeout\n");
	return(-1);
}

static int ide_packet(int drive
		, const void *pkt
		, unsigned int pkt_size
		, unsigned char *buf
		, unsigned int buf_size)
{
	unsigned short ide_base;
	unsigned char status;

	ide_base = IDE_PRIMARY_BASE - ((drive/2) *0x80);

	/* wait for ide rdy */
	if (ide_wait(ide_not_busy,ide_base, 100))
		return(-1);
	/* command type = packet */
	ide_outpb(0x00, IDE_FEATURE);
	ide_outpb(0x00, IDE_NSECTOR);
	ide_outpb(0x00, IDE_SECTOR);
	ide_outpb(buf_size&0xff, IDE_CYL_LSB);
	ide_outpb((buf_size>>8)&0xff, IDE_CYL_MSB);
	ide_outpb(IDE_SELECT_DEFAULT|(IDE_SELECT_SLAVE*(drive%2)), IDE_SELECT);
	ide_outpb(IDE_CMD_PACKET,IDE_CMD);
	
	/* wait for ide ready */
	if (ide_wait(ide_not_busy,ide_base, 100))
		return(-1);
	/* read status */
	status = ide_inpb(IDE_STATUS);
	if (!(status&IDE_STATUS_DRQ))
	{
		printk("ide_packet: error - no drq after cmd\n");
		return(-1);
	}
	/* send packet */
	ide_outpsw(pkt, pkt_size/2, IDE_DATA);
	/* wait for ide ready */
	if (ide_wait(ide_not_busy,ide_base,100))
		return(-1);
	/* read status */
	status = ide_inpb(IDE_STATUS);
	/* if nothing to read exit */
	if (buf_size == 0 ) {
		/* nothing to read, but ide drq set */
		if (status & IDE_STATUS_DRQ) {
			printk("ide_packet: error - drq but buf size=0\n");
			return(-1);
		}
		return(0);
	}
	/* something expected but drq not set */
	if (!(status & IDE_STATUS_DRQ)){
		printk("ide_packet: error - no drq but buf size >0\n");
		return(-1);
	}
	/* read data into buf */
	ide_inpsw(IDE_DATA, buf, buf_size/2);
	/* read status*/
	status = ide_inpb(IDE_STATUS);
	/* check if drq off */
	if (status&IDE_STATUS_DRQ)
	{
		printk("ide_packet: error - drq after inpsw\n");
		return(-1);
	}
	/*
	{
		int i;

		printk("ide_packet: buf= ");
		for (i=0;i<buf_size;i++) {
			printk("0x%x ", buf[i]);
		}
		printk("\n");
	}
	*/
	return(0);
}

static int ide_pkt_read_capacity(int drive)
{
	unsigned char packet[12];
	unsigned char buf[8];

	memset(packet,0,sizeof(packet));
	packet[0]=ATAPI_READ_CAPACITY;
	
	if (ide_packet(drive, packet, sizeof(packet), buf, sizeof(buf)))
		return(-1);
	printk("ide_pkt_read_capacity: sectors= %d, bytes/sector=%d\n"
		,be2le(buf,4), be2le(buf+6,2));
	return(0);	
}

static int ide_pkt_play_audio(int drive)
{
	unsigned char packet[12];

	memset(packet,0,sizeof(packet));
	packet[0]=ATAPI_PLAY_AUDIO_10;
	/* start LBA */
	packet[2]=packet[3]=packet[4]=packet[5];
	/* length */
	packet[7]=packet[8]=0xff;

	if (ide_packet(drive, packet, sizeof(packet), 0, 0))
		return(-1);
	return(0);
}

static int ide_pkt_req_sense(int drive)
{
	unsigned char packet[12];
	unsigned char buf[18];

	memset(packet,0,sizeof(packet));
	packet[0]=ATAPI_REQUEST_SENSE;
	/* allocation length */
	packet[4]=18;

	if (ide_packet(drive, packet, sizeof(packet), buf, sizeof(buf)))
		return(-1);

	printk("ide_pkt_req_sense: Sense Key= 0x%x, ASC= 0x%x, ASCQ= 0x%x"
			, buf[2], buf[12], buf[13]);
	return(0);	
}

void ide_init()
{
	printk("ide: init\n");

	ide_pkt_read_capacity(2);
	ide_pkt_req_sense(2);
	ide_pkt_play_audio(2);
	ide_pkt_req_sense(2);
}

