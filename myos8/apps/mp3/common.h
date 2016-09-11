/* common.h */

#ifndef _COMMON_H_
#define _COMMON_H_
#include "mpega.h"

typedef struct
{
	unsigned char *byte;
	int bit_idx;
}bitstream_t;

unsigned short getbits_short(bitstream_t *bs,int n);
unsigned char getbits_char(bitstream_t *bs,int n);
unsigned char get1bit(bitstream_t *bs);
int decode_header(bitstream_t *bs,frame_params_t *fr_ps);
void print_header(frame_params_t *fr_ps);
int get_time(frame_params_t *fr_ps,int frame);

#endif

