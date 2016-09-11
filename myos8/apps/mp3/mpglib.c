/* mpglib.c */

#include <stdio.h>
#include <stdlib.h>
#include "mpglib.h"

int addbuf(buf_t **list, void *buf, int len)
{
	buf_t *tmp,*p;

	if (!list || len<=0)
		return(MP_ERR);
	tmp=(buf_t*)malloc(sizeof(buf_t));
	tmp->len=len;
	tmp->next=NULL;
	tmp->buf=malloc(len);
	memcpy(tmp->buf,buf,len);
	if (*list==NULL)
	{
		*list=tmp;
		return(MP_OK);
	}
	for (p=*list;p->next!=NULL;p=p->next);
	p->next=tmp;
	return(MP_OK);
}

int removebuf(buf_t **list)
{
	buf_t *tmp;

	if (!list || !*list)
		return(MP_ERR);
	tmp=*list;
	*list=tmp->next;
	free(tmp->buf);
	free(tmp);
	return(MP_OK);
}

int readstr(mpstr_t *mpstr, void *buf, int len)
{
	int n,offset;

	if ( ( mpstr->len - mpstr->offset ) < len )
		return (MP_ERR);
	offset=0;
	while (len>0)
	{
		n= mpstr->str->len - mpstr->offset;
		if (len<n)
			n=len;
		memcpy(buf + offset, mpstr->str->buf + mpstr->offset,n);
		offset+=n;
		len-=n;
		mpstr->offset+=n;
		if (mpstr->offset >= mpstr->str->len)
		{
			mpstr->len-=mpstr->str->len;
			mpstr->offset=0;
			removebuf(&(mpstr->str));
		}
	}
	return(MP_OK);
}

int init_mpga(mpstr_t *mpstr)
{
	mpstr->read_hdr=0;
	mpstr->offset=0;
	mpstr->len=0;
	mpstr->str=NULL;
	return(MP_OK);
}

int decode_mpga(mpstr_t *mpstr, void *bufin, int bufinlen, void *bufout, int *bufoutlen)
{
	bitstream_t *bs=&(mpstr->bs);
	frame_params_t *fr_ps=&(mpstr->fr_ps);
	unsigned char frame[512];

	addbuf(&(mpstr->str),bufin,bufinlen);
	if (bufinlen>0);
		mpstr->len+=bufinlen;
	
	if (!mpstr->read_hdr)
	{
		if (readstr(mpstr,frame,4))
			return (MP_NEED_MORE);
		mpstr->read_hdr=1;
		
		bs->byte=frame;
		bs->bit_idx=0;
		decode_header(bs,fr_ps);
		print_header(fr_ps);
	}
	if (readstr(mpstr,frame,fr_ps->frame_length-4))
		return(MP_NEED_MORE);
	bs->byte=frame;
	bs->bit_idx=0;
	switch(fr_ps->header.layer)
	{
		case 1:
			do_layer1(bs,fr_ps,bufout,bufoutlen);
			break;
		case 2:
			do_layer2(bs,fr_ps,bufout,bufoutlen);
			break;
	}
	*bufoutlen*=2;
	mpstr->read_hdr=0;
	return(MP_OK);
}
