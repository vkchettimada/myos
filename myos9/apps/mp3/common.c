#include <stdio.h>
#include "mpega.h"
#include "common.h"

extern int bitrate[3][16];
extern int sfreq[3][3];

unsigned short getbits_short(bitstream_t *bs,int n)
{
	unsigned long value;

	value=bs->byte[0];
	value<<=8;
	value|=bs->byte[1];
	value<<=8;
	value|=bs->byte[2];
	value<<=bs->bit_idx;
	value&=0xffffff;
	
	bs->bit_idx+=n;
	bs->byte+=(bs->bit_idx>>3);
	bs->bit_idx&=7;
	return(value>>(24-n));
}

unsigned char getbits_char(bitstream_t *bs,int n)
{
	unsigned short value;

	value=bs->byte[0];
	value<<=8;
	value|=bs->byte[1];
	value<<=bs->bit_idx;
	
	bs->bit_idx+=n;
	bs->byte+=(bs->bit_idx>>3);
	bs->bit_idx&=7;
	return(value>>(16-n));
}
unsigned char get1bit(bitstream_t *bs)
{
	unsigned char value=(*(bs->byte))<<bs->bit_idx;

	bs->bit_idx++;
	bs->byte+=(bs->bit_idx>>3);
	bs->bit_idx&=7;
	return(value>>7);
}

int decode_header(bitstream_t *bs,frame_params_t *fr_ps)
{
	static int jsb_table[3][4]={{4,8,12,16},{4,8,12,16},{0,4,8,16}};
	frame_header_t *header=&fr_ps->header;

	if ((getbits_short(bs,12)&0xfff)==0xfff)
	{
		header->version=get1bit(bs);
		header->layer=4-getbits_char(bs,2);
		header->error_protection=!get1bit(bs);
		header->bitrate_index=getbits_char(bs,4);
		header->sfreq_index=getbits_char(bs,2);
		header->padding=get1bit(bs);
		header->extension=get1bit(bs);
		header->mode=getbits_char(bs,2);
		header->mode_ext=getbits_char(bs,2);
		header->copyright=get1bit(bs);
		header->original=get1bit(bs);
		header->emphasis=get1bit(bs);

		switch(header->layer)
		{
			case 1:
	/* frame length = (12 * bitrate / samplerate + padding ) * 4 ; */
				fr_ps->frame_length = \
		( 12 * bitrate[header->layer-1][header->bitrate_index] * 1000 \
		  / sfreq[0][header->sfreq_index] \
		  + header->padding )
		* 4 ;
				break;
			case 2:
			case 3:
	/* frame length = 144 * bitrate / samplerate + padding ; */
				fr_ps->frame_length= 144 \
		* bitrate[header->layer-1][header->bitrate_index] *1000 \
		/ sfreq[0][header->sfreq_index] \
		+ header->padding ;
				break;
		}

		fr_ps->stereo=(header->mode==MPG_MD_MONO)?1:2;
		if (header->mode==MPG_MD_JOINT_STEREO)
			fr_ps->jsbound=jsb_table[header->layer-1][header->mode_ext];
		else
			fr_ps->jsbound=SBLIMIT;
	}
	else
	{
		fprintf(stderr,"\nError in frame sync.\n");
		return(-1);
	}
	return(0);
}

void print_header(frame_params_t *fr_ps)
{
	static char *modes[4]={"Stereo","Joint-Stereo","Dual-Channel","Mono" };
	static char *mode_exts[4]={"LR_LR","LR_I","MS_LR","MS_I"};
	frame_header_t *header=&fr_ps->header;

	fprintf(stderr,"\rMPEG %s, ",header->version?"1":"2");
	fprintf(stderr,"Layer %d, ",header->layer);
	fprintf(stderr,"%dkbps, ",bitrate[header->layer-1][header->bitrate_index]);
	fprintf(stderr,"%dHz, ",sfreq[0][header->sfreq_index]);
	fprintf(stderr,"%s, ",modes[header->mode]);
	fprintf(stderr,"%s, ",mode_exts[header->mode_ext]);
	fprintf(stderr,"len=%d, ",fr_ps->frame_length);
}

int get_time(frame_params_t *fr_ps,int frame)
{
	int samples_per_frame[3]={384,1152,1152};
	int t;
	
	t=frame * samples_per_frame[fr_ps->header.layer-1];
	t/=sfreq[0][fr_ps->header.sfreq_index];
	return(t);
}
