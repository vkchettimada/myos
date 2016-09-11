/* layer2.c */
#include "mpega.h"
#include "common.h"

extern int sblim[];
extern int *nbal[4];
extern int translate[3][2][16];
extern double c[17];
extern double d[17];
extern double multiple[64];

void II_select_alloc(frame_params_t *fr_ps)
{
	int table;
	frame_header_t *header=&fr_ps->header;
	
	table=translate[header->sfreq_index][fr_ps->stereo][header->bitrate_index];
	fr_ps->nbal=nbal[table];
	fr_ps->sblimit=sblim[table];
	read_bit_alloc(table,&fr_ps->alloc);
}

/* Get bit allocation */
void II_decode_bitalloc(bitstream_t *bs,frame_params_t *fr_ps,int bitalloc[2][SBLIMIT])
{
	int sb,ch;
	int stereo=fr_ps->stereo;
	int jsbound=fr_ps->jsbound;
	int sblimit=fr_ps->sblimit;
	int *nbal=fr_ps->nbal;
	int b;

	for (sb=0;sb<jsbound;sb++)
		for (ch=0;ch<stereo;ch++)
			bitalloc[ch][sb]=getbits_char(bs,nbal[sb]);
	for (;sb<sblimit;sb++)
	{
		b=getbits_char(bs,nbal[sb]);
		for (ch=0;ch<stereo;ch++)
			bitalloc[ch][sb]=b;
	}
	for (;sb<SBLIMIT;sb++)
		for (ch=0;ch<stereo;ch++)
			bitalloc[ch][sb]=0;
}

/* get scale factor selection index */
void II_decode_scfsi(bitstream_t *bs, frame_params_t *fr_ps, int bitalloc[2][SBLIMIT], int scfsi[2][SBLIMIT])
{
	int sb,ch;
	int stereo=fr_ps->stereo;
	int sblimit=fr_ps->sblimit;

	for (sb=0;sb<sblimit;sb++)
		for (ch=0;ch<stereo;ch++)
			if (bitalloc[ch][sb])
				scfsi[ch][sb]=getbits_char(bs,2);
}

void II_decode_scale_index(bitstream_t *bs, frame_params_t *fr_ps, int bitalloc[2][SBLIMIT], int scfsi[2][SBLIMIT], int scale_index[2][3][SBLIMIT])
{
	int sb,ch;
	int stereo=fr_ps->stereo;
	int sblimit=fr_ps->sblimit;
	
	for (sb=0;sb<sblimit;sb++)
		for (ch=0;ch<stereo;ch++)
			if (!bitalloc[ch][sb])
			{
				scale_index[ch][0][sb]=
				scale_index[ch][1][sb]=
				scale_index[ch][2][sb]=SCALE_RANGE-1;
			}
			else
			{
				switch(scfsi[ch][sb])
				{
				case 0:
					scale_index[ch][0][sb]=getbits_char(bs,6);
					scale_index[ch][1][sb]=getbits_char(bs,6);
					scale_index[ch][2][sb]=getbits_char(bs,6);
					break;
				case 1:
					scale_index[ch][0][sb]=
					scale_index[ch][1][sb]=getbits_char(bs,6);
					scale_index[ch][2][sb]=getbits_char(bs,6);
					break;
				case 2:
					scale_index[ch][0][sb]=
					scale_index[ch][1][sb]=
					scale_index[ch][2][sb]=getbits_char(bs,6);
					break;
				case 3:
					scale_index[ch][0][sb]=getbits_char(bs,6);
					scale_index[ch][1][sb]=
					scale_index[ch][2][sb]=getbits_char(bs,6);
					break;
				}
			}
	for (;sb<SBLIMIT;sb++)
		for (ch=0;ch<stereo;ch++)
			scale_index[ch][0][sb]=
			scale_index[ch][1][sb]=
			scale_index[ch][2][sb]=SCALE_RANGE-1;
}

void II_decode_sample(bitstream_t *bs,frame_params_t *fr_ps,int bitalloc[2][SBLIMIT],int sample[2][3][SBLIMIT])
{
	int sb,ch;
	int stereo=fr_ps->stereo;
	int jsbound=fr_ps->jsbound;

	for (sb=0;sb<SBLIMIT;sb++)
		for (ch=0;ch<((sb<jsbound)?stereo:1);ch++)
		{
			if (bitalloc[ch][sb])
			{
				int bits,group;

				bits=fr_ps->alloc[sb][bitalloc[ch][sb]].bits;
				group=fr_ps->alloc[sb][bitalloc[ch][sb]].group;
				if (group==3)
				{
					sample[ch][0][sb]=getbits_char(bs,bits);
					sample[ch][1][sb]=getbits_char(bs,bits);
					sample[ch][2][sb]=getbits_char(bs,bits);
				}
				else
				{
					uint steps,c;

					c=getbits_char(bs,bits);
					steps=fr_ps->alloc[sb][bitalloc[ch][sb]].steps;
					sample[ch][0][sb]=c%steps;
					sample[ch][1][sb]=(c/=steps)%steps;
					sample[ch][2][sb]=(c/=steps)%steps;
				}
			}
			else
				sample[ch][0][sb]=
					sample[ch][1][sb]=
					sample[ch][2][sb]=0;
			if (stereo==2 && sb >= jsbound)
			{
				sample[1][0][sb]=sample[0][0][sb];
				sample[1][1][sb]=sample[0][1][sb];
				sample[1][2][sb]=sample[0][2][sb];
			}
		}
}

void II_dequantize_sample(frame_params_t *fr_ps,int bitalloc[2][SBLIMIT],int sample[2][3][SBLIMIT],double fraction[2][3][SBLIMIT])
{
    int sb,ch,grp;
    int sblimit = fr_ps->sblimit;
    int stereo = fr_ps->stereo;
    int x;
    
    for (sb=0;sb<sblimit;sb++)
	    for (grp=0;grp<3;grp++)
		    for (ch=0;ch<stereo;ch++)
			    if (bitalloc[ch][sb])
			    {
		    x = 0;
		    while ((1L<<x) < fr_ps->alloc[sb][bitalloc[ch][sb]].steps)
			x++;
		    if (((sample[ch][grp][sb] >> (x-1)) & 1) == 1)
			fraction[ch][grp][sb] = 0.0;
		    else
			fraction[ch][grp][sb] = -1.0;
		    fraction[ch][grp][sb] +=
			(double)(sample[ch][grp][sb] & ((1<<(x-1))-1)) /
			(double)(1L<<(x-1));
		    fraction[ch][grp][sb] += d[fr_ps->alloc[sb][bitalloc[ch][sb]].quant];
		    fraction[ch][grp][sb] *= c[fr_ps->alloc[sb][bitalloc[ch][sb]].quant];
		    }
		    else
			    fraction[ch][grp][sb] = 0.0;
    for (;sb<SBLIMIT;sb++)
	    for (grp=0;grp<3;grp++)
		    for (ch=0;ch<stereo;ch++)
			    fraction[ch][grp][sb] = 0.0;
}

void II_denormalize_sample(frame_params_t *fr_ps,int scale_index[2][3][SBLIMIT],double fraction[2][3][SBLIMIT],int x)
{
    int sb,ch;
    int stereo = fr_ps->stereo;

    for (sb=0;sb<SBLIMIT;sb++)
	    for (ch=0;ch<stereo;ch++)
	    {
		    double m;

		    m=multiple[scale_index[ch][x][sb]];
		    fraction[ch][0][sb]*=m;
		    fraction[ch][1][sb]*=m;
		    fraction[ch][2][sb]*=m;
	    }
}

void do_layer2(bitstream_t *bs, frame_params_t *fr_ps, short pcm_out[2*SCALE_BLOCK*3*SBLIMIT],int *len)
{
	int bitalloc[2][SBLIMIT];
	int scfsi[2][SBLIMIT];
	int scale_index[2][3][SBLIMIT];
	int sample[2][3][SBLIMIT];
	double fraction[2][3][SBLIMIT];
	short pcm_sample[2][SCALE_BLOCK][3][SBLIMIT];
	int i,j,k,l;

	II_select_alloc(fr_ps);
	II_decode_bitalloc(bs,fr_ps,bitalloc);
	II_decode_scfsi(bs,fr_ps,bitalloc,scfsi);
	II_decode_scale_index(bs,fr_ps,bitalloc,scfsi,scale_index);
	*len=0;
	for (i=0;i<SCALE_BLOCK;i++)
	{
		II_decode_sample(bs,fr_ps,bitalloc,sample);
		II_dequantize_sample(fr_ps,bitalloc,sample,fraction);
		II_denormalize_sample(fr_ps,scale_index,fraction,i>>2);
		for (k=0;k<3;k++)
		for (j=0;j<fr_ps->stereo;j++)
			SubBandSynthesis(fraction[j][k],j,pcm_sample[j][i][k]);
	}
	for (i=0;i<SCALE_BLOCK;i++)
		for (l=0;l<3;l++)
		for (j=0;j<SBLIMIT;j++)
			for (k=0;k<fr_ps->stereo;k++)
				pcm_out[(*len)++]=pcm_sample[k][i][l][j];
}

