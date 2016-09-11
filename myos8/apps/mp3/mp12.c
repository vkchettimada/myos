
#include <stdio.h>
#include "mpglib.h"

#define BUFLEN 1024

int main()
{
	mpstr_t mp_str;
	char bufin[BUFLEN],bufout[2*3*SBLIMIT*SCALE_BLOCK];
	int len,retcode;
	
	init_mpga(&mp_str);
	while (read(0,bufin,BUFLEN)==BUFLEN)
	{
		retcode=decode_mpga(&mp_str,bufin,BUFLEN,bufout,&len);
		/*addbuf(&(mp_str.str),bufin,BUFLEN);
		mp_str.len+=BUFLEN;*/
		
		while (retcode==MP_OK)
		{
			write(1,bufout,len);
			retcode=decode_mpga(&mp_str,NULL,0,bufout,&len);
		}
	}
	return(0);
}


