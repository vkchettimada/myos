/* huffman.c */

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "huffman.h"
     
HUFFBITS dmask = 1 << (sizeof(HUFFBITS)*8-1);
unsigned int hs = sizeof(HUFFBITS)*8;

extern huffcodetab_t ht[HTN];	/* array of all huffcodtable headers	*/
				/* 0..31 Huffman code table 0..31	*/
				/* 32,33 count1-tables			*/

/* do the huffman-decoding 						*/
/* note! for counta,countb -the 4 bit value is returned in y, discard x */
int huffman_decoder(bitstream_t *bs, huffcodetab_t *h, int *x, int *y, int *v, int *w)
{  
  HUFFBITS level;
  int point = 0;
  int error = 1;
  level     = dmask;
  if (h->val == NULL) return 2;

  /* table 0 needs no bits */
  if ( h->treelen == 0)
  {  *x = *y = 0;
     return 0;
  }


  /* Lookup in Huffman table. */

  do {
    if (h->val[point][0]==0) {   /*end of tree*/
      *x = h->val[point][1] >> 4;
      *y = h->val[point][1] & 0xf;

      error = 0;
      break;
    } 
    if (get1bit(bs)) {
      while (h->val[point][1] >= MXOFF) point += h->val[point][1]; 
      point += h->val[point][1];
    }
    else {
      while (h->val[point][0] >= MXOFF) point += h->val[point][0]; 
      point += h->val[point][0];
    }
    level >>= 1;
  } while (level  || (point < ht->treelen) );
  
  /* Check for error. */
  
  if (error) { /* set x and y to a medium value as a simple concealment */
    fprintf(stderr,"Illegal Huffman code in data.\n");
    *x = (h->xlen-1 << 1);
    *y = (h->ylen-1 << 1);
  }

  /* Process sign encodings for quadruples tables. */

  if (h->tablename[0] == '3'
      && (h->tablename[1] == '2' || h->tablename[1] == '3')) {
     *v = (*y>>3) & 1;
     *w = (*y>>2) & 1;
     *x = (*y>>1) & 1;
     *y = *y & 1;

     /* v, w, x and y are reversed in the bitstream. 
        switch them around to make test bistream work. */
     
/*   {int i=*v; *v=*y; *y=i; i=*w; *w=*x; *x=i;}  MI */

     if (*v)
        if (get1bit(bs) == 1) *v = -*v;
     if (*w)
        if (get1bit(bs) == 1) *w = -*w;
     if (*x)
        if (get1bit(bs) == 1) *x = -*x;
     if (*y)
        if (get1bit(bs) == 1) *y = -*y;
     }
     
  /* Process sign and escape encodings for dual tables. */
  
  else {
  
      /* x and y are reversed in the test bitstream.
         Reverse x and y here to make test bitstream work. */
	 
/*    removed 11/11/92 -ag  
		{int i=*x; *x=*y; *y=i;} 
*/      
     if (h->linbits)
       if ((h->xlen-1) == *x) 
         *x += getbits_short(bs,h->linbits);
     if (*x)
        if (get1bit(bs) == 1) *x = -*x;
     if (h->linbits)	  
       if ((h->ylen-1) == *y)
         *y += getbits_short(bs,h->linbits);
     if (*y)
        if (get1bit(bs) == 1) *y = -*y;
     }
	  
  return error;  
}
