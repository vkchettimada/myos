/* mpega.h */

#ifndef _MPEGA_H_
#define _MPEGA_H_

#include <sys/types.h>

typedef struct
{
	int version;
	int layer;
	int error_protection;
	int bitrate_index;
	int sfreq_index;
	int padding;
	int extension;
	int mode;
#define MPG_MD_STEREO		0
#define MPG_MD_JOINT_STEREO	1
#define MPG_MD_DUAL_STEREO	2
#define MPG_MD_MONO		3
	int mode_ext;
#define MPG_MD_EXT_LR_LR	0
#define MPG_MD_EXT_LR_I		1
#define MPG_MD_EXT_MS_LR	2
#define MPG_MD_EXT_MS_I		3
	int copyright;
	int original;
	int emphasis;
}frame_header_t;

typedef struct
{
	unsigned part2_3_length;
	unsigned big_values;
	unsigned global_gain;
	unsigned scalefac_compress;
	unsigned window_switching_flag;
	unsigned block_type;
	unsigned mixed_block_flag;
	unsigned table_select[3];
	unsigned subblock_gain[3];
	unsigned region0_count;
	unsigned region1_count;
	unsigned preflag;
	unsigned scalefac_scale;
	unsigned count1table_select;
}gr_info_t;

typedef struct
{
	unsigned main_data_begin;
	unsigned private_bits;
	struct
	{
		unsigned scfsi[4];
		gr_info_t gr[2];
	} ch[2];
}III_side_info_t;

typedef struct 
{
	int l[23];            // [cb]
	int s[3][13];         // [window][cb]
} III_scalefactor_t[2];  // [ch]


#define SBLIMIT		32
#define SSLIMIT		18
#define SCALE_BLOCK	12
#define SCALE_RANGE	64

#define PI 3.1415

typedef struct
{
	uint steps;
	uint bits;
	uint group;
	uint quant;
} sb_table_t;

typedef sb_table_t al_table_t[SBLIMIT][16];

typedef struct
{
	frame_header_t header;
	int frame_length;
	int stereo;
	int jsbound;
	int sblimit;
	int *nbal;
	al_table_t alloc;
}frame_params_t;

#endif

