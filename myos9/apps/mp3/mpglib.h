/* mpglib.h */
#include "mpega.h"
#include "common.h"

typedef struct _buf_t_
{
	int len;
	void *buf;
	struct _buf_t_ *next;	
} buf_t;

typedef struct
{
	bitstream_t bs;
	frame_params_t fr_ps;
	int read_hdr;
	int offset;
	int len;
	buf_t *str;
}mpstr_t;

int init_mpga(mpstr_t *mpstr);
int decode_mpga(mpstr_t *mpstr, void *bufin, int bufinlen, void *bufout, int *bufoutlen);

#define MP_OK 0
#define MP_ERR 1
#define MP_NEED_MORE 2

