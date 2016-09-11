/* layer1.c */
#include "mpega.h"
#include "common.h"

extern double multiple[64];

void I_decode_bitalloc(bitstream_t *bs,frame_params_t *fr_ps,int bitalloc[2][SBLIMIT])
{
	int i,j;
	int stereo=fr_ps->stereo;
	int jsbound=fr_ps->jsbound;
	int b;

	for (i=0;i<jsbound;i++)
		for (j=0;j<stereo;j++)
			bitalloc[j][i]=getbits_char(bs,4);
	for (;i<SBLIMIT;i++)
	{
		b=getbits_char(bs,4);
		for (j=0;j<stereo;j++)
			bitalloc[j][i]=b;
	}
}

void I_decode_scale(bitstream_t *bs,frame_params_t *fr_ps,int bitalloc[2][SBLIMIT],int scale_index[2][3][SBLIMIT])
{
	int i,j;
	int stereo=fr_ps->stereo;

	for (i=0;i<SBLIMIT;i++)
		for (j=0;j<stereo;j++)
			if (!bitalloc[j][i])
				scale_index[j][0][i]=SCALE_RANGE-1;
			else
				scale_index[j][0][i]=getbits_char(bs,6);
}

void I_decode_sample(bitstream_t *bs,frame_params_t *fr_ps,int bitalloc[2][SBLIMIT],int sample[2][3][SBLIMIT])
{
	int i,j,bits;
	int stereo=fr_ps->stereo;
	int jsbound=fr_ps->jsbound;
	unsigned int b;

	for (i=0;i<jsbound;i++)
		for (j=0;j<stereo;j++)
			if ((bits=bitalloc[j][i])==0)
				sample[j][0][i]=0;
			else
				sample[j][0][i]=getbits_short(bs,bits+1);
	for (;i<SBLIMIT;i++)
	{
		if ((bits=bitalloc[0][i])==0)
			b=0;
		else
			b=getbits_short(bs,bits+1);
		for (j=0;j<stereo;j++)
			sample[j][0][i]=b;
	}
}

void I_dequantize_sample(frame_params_t *fr_ps,int bitalloc[2][SBLIMIT],int sample[2][3][SBLIMIT],double fraction[2][3][SBLIMIT])
{
    int i,j,nb;
    int stereo = fr_ps->stereo;
    
    for (i=0;i<SBLIMIT;i++)
	    for (j=0;j<stereo;j++)
		    if ((nb=bitalloc[j][i]))
			    fraction[j][0][i]=(double)(((-1)<<nb)+sample[j][0][i]+1)/(double)(1L<<nb);
		    else
			    fraction[j][0][i] = 0.0;
}

void I_denormalize_sample(frame_params_t *fr_ps,int scale_index[2][3][SBLIMIT],double fraction[2][3][SBLIMIT])
{
    int i,j;
    int stereo = fr_ps->stereo;

    for (i=0;i<SBLIMIT;i++)
	    for (j=0;j<stereo;j++)
		    fraction[j][0][i]*=multiple[scale_index[j][0][i]];
}

void do_layer1(bitstream_t *bs, frame_params_t *fr_ps, short pcm_out[2*SCALE_BLOCK*SBLIMIT],int *len)
{
	int bitalloc[2][SBLIMIT];
	int scale_index[2][3][SBLIMIT];
	int sample[2][3][SBLIMIT];
	double fraction[2][3][SBLIMIT];
	short pcm_sample[2][SCALE_BLOCK][SBLIMIT];
	int i,j,k;
	
	I_decode_bitalloc(bs,fr_ps,bitalloc);
	I_decode_scale(bs,fr_ps,bitalloc,scale_index);
	*len=0;
	for (i=0;i<SCALE_BLOCK;i++)
	{
		I_decode_sample(bs,fr_ps,bitalloc,sample);
		I_dequantize_sample(fr_ps,bitalloc,sample,fraction);
		I_denormalize_sample(fr_ps,scale_index,fraction);
		for (j=0;j<fr_ps->stereo;j++)
			SubBandSynthesis(fraction[j][0],j,pcm_sample[j][i]);
	}
	for (i=0;i<SCALE_BLOCK;i++)
		for (j=0;j<SBLIMIT;j++)
			for (k=0;k<fr_ps->stereo;k++)
				pcm_out[(*len)++]=pcm_sample[k][i][j];
}

