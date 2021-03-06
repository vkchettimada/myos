#include <stdio.h>
#include <math.h>
#include "common.h"
#include "huffman.h"

void III_get_side_info(bitstream_t *bs,frame_params_t *fr_ps,III_side_info_t *si)
{
	int ch, gr, reg, win, i;
	int stereo = fr_ps->stereo;

	si->main_data_begin=getbits_short(bs, 9);
	if (stereo==1)
		si->private_bits=getbits_char(bs,5);
	else
		si->private_bits=getbits_char(bs,3);
	
	for (ch=0;ch<stereo;ch++)
		for (i=0;i<4;i++)
			si->ch[ch].scfsi[i]=get1bit(bs);
	for (gr=0;gr<2;gr++)
	{
		for (ch=0;ch<stereo;ch++) 
		{
			si->ch[ch].gr[gr].part2_3_length=getbits_short(bs,12);
			si->ch[ch].gr[gr].big_values=getbits_short(bs,9);
			si->ch[ch].gr[gr].global_gain=getbits_char(bs,8);
			si->ch[ch].gr[gr].scalefac_compress=getbits_char(bs,4);
			si->ch[ch].gr[gr].window_switching_flag=get1bit(bs);
			if (si->ch[ch].gr[gr].window_switching_flag) 
			{
				si->ch[ch].gr[gr].block_type=getbits_char(bs,2);
				si->ch[ch].gr[gr].mixed_block_flag=get1bit(bs);
				for (reg=0;reg<2;reg++)
					si->ch[ch].gr[gr].table_select[reg]=getbits_char(bs,5);
				for (win=0;win<3;win++)
					si->ch[ch].gr[gr].subblock_gain[win]=getbits_char(bs,3);
				/* Set region_count parameters since they are implicit in this case. */
				if (si->ch[ch].gr[gr].block_type==0)
				{
					fprintf(stderr,"Side info bad: block_type == 0 in split block.\n");
					exit(0);
				}
				else if ((si->ch[ch].gr[gr].block_type==2) && (si->ch[ch].gr[gr].mixed_block_flag==0))
					si->ch[ch].gr[gr].region0_count=8; /* MI 9; */
				else
					si->ch[ch].gr[gr].region0_count=7; /* MI 8; */
				si->ch[ch].gr[gr].region1_count=20-si->ch[ch].gr[gr].region0_count;
			}
			else 
			{
				for (i=0;i<3;i++)
					si->ch[ch].gr[gr].table_select[i]=getbits_char(bs,5);
				si->ch[ch].gr[gr].region0_count=getbits_char(bs,4);
				si->ch[ch].gr[gr].region1_count=getbits_char(bs,3);
				si->ch[ch].gr[gr].block_type=0;
			}      
			si->ch[ch].gr[gr].preflag=get1bit(bs);
			si->ch[ch].gr[gr].scalefac_scale=get1bit(bs);
			si->ch[ch].gr[gr].count1table_select=get1bit(bs);
		}
	}
}

struct {
   int l[5];
   int s[3];
} sfbtable = {
	{0, 6, 11, 16, 21},
	{0, 6, 12}
};
                         
int slen[2][16] = {
	{0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
	{0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}
};

void III_get_scale_factors(bitstream_t *bs,frame_params_t *fr_ps,III_side_info_t *si,int ch,int gr,III_scalefactor_t *scale)
{
	int sfb, i, window;
	gr_info_t *gr_info=&(si->ch[ch].gr[gr]);
	
	if (gr_info->window_switching_flag && (gr_info->block_type==2)) 
	{
		if (gr_info->mixed_block_flag)
		{
			for (sfb=0;sfb<8;sfb++)
				(*scale)[ch].l[sfb]=getbits_short(bs,slen[0][gr_info->scalefac_compress]);
			for (sfb=3;sfb<6;sfb++)
				for (window=0;window<3;window++)
					(*scale)[ch].s[window][sfb]=getbits_short(bs,slen[0][gr_info->scalefac_compress]);
			for (sfb=6;sfb<12;sfb++)
				for (window=0;window<3;window++)
					(*scale)[ch].s[window][sfb]=getbits_short(bs,slen[1][gr_info->scalefac_compress]);
			for (sfb=12,window=0;window<3;window++)
				(*scale)[ch].s[window][sfb]=0;
		}
		else 
		{
			for (i=0;i<2;i++) 
				for (sfb=sfbtable.s[i];sfb<sfbtable.s[i+1];sfb++)
					for (window=0;window<3;window++)
						(*scale)[ch].s[window][sfb]=getbits_short(bs,slen[i][gr_info->scalefac_compress]);
			for (sfb=12,window=0;window<3;window++)
				(*scale)[ch].s[window][sfb]=0;
		}
	}          
	else
	{
		for (i=0;i<4;i++) 
		{          
			if ((si->ch[ch].scfsi[i]==0)||(gr==0))
				for (sfb=sfbtable.l[i];sfb<sfbtable.l[i+1];sfb++)
					(*scale)[ch].l[sfb]=getbits_short(bs,slen[(i<2)?0:1][gr_info->scalefac_compress]);
		}
		(*scale)[ch].l[22]=0; 
	}
}

struct  {
   int l[23];
   int s[14];} sfBandIndex[6] =
    {{{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
     {0,4,8,12,18,24,32,42,56,74,100,132,174,192}},
    {{0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278,330,394,464,540,576},
     {0,4,8,12,18,26,36,48,62,80,104,136,180,192}},
    {{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
     {0,4,8,12,18,26,36,48,62,80,104,134,174,192}},

    {{0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
     {0,4,8,12,16,22,30,40,52,66,84,106,136,192}},
    {{0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
     {0,4,8,12,16,22,28,38,50,64,80,100,126,192}},
    {{0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576},
     {0,4,8,12,16,22,30,42,58,78,104,138,180,192}}};

void III_hufman_decode(bitstream_t *bs, frame_params_t *fr_ps,III_side_info_t *si,int ch,int gr,int part2_start,int is[SBLIMIT][SSLIMIT])
{
   int i,v,w,x,y,region1Start,region2Start,sfreq,currentBit,grBits;
   huffcodetab_t *h;
   static int init=0;
   
   if (!init)
   {
	   tables_read_decoder_table();
	   init=1;
   }
   sfreq = fr_ps->header.sfreq_index + (fr_ps->header.version * 3);

   /* Find region boundary for short block case. */
   
   if ( ((*si).ch[ch].gr[gr].window_switching_flag) && 
        ((*si).ch[ch].gr[gr].block_type == 2) ) { 
   
      /* Region2. */
        region1Start = 36;  /* sfb[9/3]*3=36 */
        region2Start = 576; /* No Region2 for short block case. */
   }
   else {          /* Find region boundary for long block case. */

      region1Start = sfBandIndex[sfreq]
                           .l[(*si).ch[ch].gr[gr].region0_count + 1]; /* MI */
      region2Start = sfBandIndex[sfreq]
                              .l[(*si).ch[ch].gr[gr].region0_count +
                              (*si).ch[ch].gr[gr].region1_count + 2]; /* MI */
      }

   /* Read bigvalues area. */
   for (i=0; i<(*si).ch[ch].gr[gr].big_values*2; i+=2)
   {
      if (i<region1Start)
	      h = &ht[(*si).ch[ch].gr[gr].table_select[0]];
      else if (i<region2Start) 
	      h = &ht[(*si).ch[ch].gr[gr].table_select[1]];
      else 
	      h = &ht[(*si).ch[ch].gr[gr].table_select[2]];
      huffman_decoder(h, &x, &y, &v, &w);
      is[i/SSLIMIT][i%SSLIMIT] = x;
      is[(i+1)/SSLIMIT][(i+1)%SSLIMIT] = y;
   }

   /* Read count1 area. */
   h = &ht[(*si).ch[ch].gr[gr].count1table_select+32];
   while ( ( hsstell() < ( part2_start + (*si).ch[ch].gr[gr].part2_3_length ) ) && ( i < SSLIMIT*SBLIMIT ) )
   {
	   huffman_decoder(h, &x, &y, &v, &w);
	   is[i/SSLIMIT][i%SSLIMIT] = v;
	   is[(i+1)/SSLIMIT][(i+1)%SSLIMIT] = w;
	   is[(i+2)/SSLIMIT][(i+2)%SSLIMIT] = x;
	   is[(i+3)/SSLIMIT][(i+3)%SSLIMIT] = y;
	   i += 4;
   }

   if ( hsstell() > (part2_start + (*si).ch[ch].gr[gr].part2_3_length ) )
   {
	   i -=4;
	   rewindNbits(hsstell()-part2_start - (*si).ch[ch].gr[gr].part2_3_length);
   }

   /* Dismiss stuffing Bits */
   grBits     = part2_start + (*si).ch[ch].gr[gr].part2_3_length;
   currentBit = hsstell();
   if ( currentBit < grBits )
	   getbits_short( bs, grBits - currentBit );
   
   /* Zero out rest. */
   for (; i<SSLIMIT*SBLIMIT; i++)
	   is[i/SSLIMIT][i%SSLIMIT] = 0;
}


int pretab[22] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0};

void III_dequantize_sample(is,xr,scalefac,gr_info, ch,fr_ps)
long int is[SBLIMIT][SSLIMIT];
double xr[SBLIMIT][SSLIMIT];
gr_info_t *gr_info;
III_scalefactor_t *scalefac;
frame_params_t *fr_ps;
int ch;
{
   int ss,sb,cb=0,sfreq;
   int stereo = fr_ps->stereo;
   int next_cb_boundary, cb_begin, cb_width, sign;

   sfreq=fr_ps->header.sfreq_index + (fr_ps->header.version * 3);

   /* choose correct scalefactor band per block type, initalize boundary */

   if (gr_info->window_switching_flag && (gr_info->block_type == 2) )
      if (gr_info->mixed_block_flag) 
         next_cb_boundary=sfBandIndex[sfreq].l[1];  /* LONG blocks: 0,1,3 */
      else {
         next_cb_boundary=sfBandIndex[sfreq].s[1]*3; /* pure SHORT block */
    cb_width = sfBandIndex[sfreq].s[1];
    cb_begin = 0;
      }  
   else 
      next_cb_boundary=sfBandIndex[sfreq].l[1];  /* LONG blocks: 0,1,3 */

   /* apply formula per block type */

   for (sb=0 ; sb < SBLIMIT ; sb++)
      for (ss=0 ; ss < SSLIMIT ; ss++) {

         if ( (sb*18)+ss == next_cb_boundary)  { /* Adjust critical band boundary */
            if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {
               if (gr_info->mixed_block_flag)  {
                  if (((sb*18)+ss) == sfBandIndex[sfreq].l[8])  {
                     next_cb_boundary=sfBandIndex[sfreq].s[4]*3; 
                     cb = 3;
                     cb_width = sfBandIndex[sfreq].s[cb+1] - 
                                sfBandIndex[sfreq].s[cb];
                     cb_begin = sfBandIndex[sfreq].s[cb]*3;      
                  }
                  else if (((sb*18)+ss) < sfBandIndex[sfreq].l[8]) 
                     next_cb_boundary = sfBandIndex[sfreq].l[(++cb)+1];
                  else {
                     next_cb_boundary = sfBandIndex[sfreq].s[(++cb)+1]*3;
                     cb_width = sfBandIndex[sfreq].s[cb+1] - 
                                    sfBandIndex[sfreq].s[cb];
                     cb_begin = sfBandIndex[sfreq].s[cb]*3;      
                  }   
               }
               else  {
                  next_cb_boundary = sfBandIndex[sfreq].s[(++cb)+1]*3;
                  cb_width = sfBandIndex[sfreq].s[cb+1] - 
                               sfBandIndex[sfreq].s[cb];
                cb_begin = sfBandIndex[sfreq].s[cb]*3;      
               } 
            }
            else /* long blocks */
               next_cb_boundary = sfBandIndex[sfreq].l[(++cb)+1];
         }

         /* Compute overall (global) scaling. */

         xr[sb][ss] = pow( 2.0 , (0.25 * (gr_info->global_gain - 210.0)));

         /* Do long/short dependent scaling operations. */
        
         if (gr_info->window_switching_flag && (
            ((gr_info->block_type == 2) && (gr_info->mixed_block_flag == 0)) ||
            ((gr_info->block_type == 2) && gr_info->mixed_block_flag && (sb >= 2)) )) {

            xr[sb][ss] *= pow(2.0, 0.25 * -8.0 * 
                    gr_info->subblock_gain[(((sb*18)+ss) - cb_begin)/cb_width]);
            xr[sb][ss] *= pow(2.0, 0.25 * -2.0 * (1.0+gr_info->scalefac_scale)
              * (*scalefac)[ch].s[(((sb*18)+ss) - cb_begin)/cb_width][cb]);
         }
         else {   /* LONG block types 0,1,3 & 1st 2 subbands of switched blocks */
            xr[sb][ss] *= pow(2.0, -0.5 * (1.0+gr_info->scalefac_scale)
                                        * ((*scalefac)[ch].l[cb]
                                        + gr_info->preflag * pretab[cb]));
         }

         /* Scale quantized value. */
        
         sign = (is[sb][ss]<0) ? 1 : 0; 
         xr[sb][ss] *= pow( (double) abs(is[sb][ss]), ((double)4.0/3.0) );
         if (sign) xr[sb][ss] = -xr[sb][ss];
      }
}

III_reorder (xr, ro, gr_info, fr_ps) 
double xr[SBLIMIT][SSLIMIT]; 
double ro[SBLIMIT][SSLIMIT]; 
gr_info_t *gr_info;
frame_params_t *fr_ps;
{
   int sfreq;
   int sfb, sfb_start, sfb_lines;
   int sb, ss, window, freq, src_line, des_line;

   sfreq=fr_ps->header.sfreq_index + (fr_ps->header.version * 3);

   for(sb=0;sb<SBLIMIT;sb++)
      for(ss=0;ss<SSLIMIT;ss++) 
         ro[sb][ss] = 0;

   if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {
      if (gr_info->mixed_block_flag) {
         /* NO REORDER FOR LOW 2 SUBBANDS */
         for (sb=0 ; sb < 2 ; sb++)
            for (ss=0 ; ss < SSLIMIT ; ss++) {
               ro[sb][ss] = xr[sb][ss];
            }
         /* REORDERING FOR REST SWITCHED SHORT */
         for(sfb=3,sfb_start=sfBandIndex[sfreq].s[3],
            sfb_lines=sfBandIndex[sfreq].s[4] - sfb_start; 
            sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
            (sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
               for(window=0; window<3; window++)
                  for(freq=0;freq<sfb_lines;freq++) {
                     src_line = sfb_start*3 + window*sfb_lines + freq; 
                     des_line = (sfb_start*3) + window + (freq*3);
                     ro[des_line/SSLIMIT][des_line%SSLIMIT] = 
                                    xr[src_line/SSLIMIT][src_line%SSLIMIT];
               }
      } 
      else {  /* pure short */
         for(sfb=0,sfb_start=0,sfb_lines=sfBandIndex[sfreq].s[1]; 
            sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
            (sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
               for(window=0; window<3; window++)
                  for(freq=0;freq<sfb_lines;freq++) {
                     src_line = sfb_start*3 + window*sfb_lines + freq; 
                     des_line = (sfb_start*3) + window + (freq*3);
                     ro[des_line/SSLIMIT][des_line%SSLIMIT] = 
                                    xr[src_line/SSLIMIT][src_line%SSLIMIT];
               }
      }
   }
   else {   /*long blocks */
      for (sb=0 ; sb < SBLIMIT ; sb++)
         for (ss=0 ; ss < SSLIMIT ; ss++) 
            ro[sb][ss] = xr[sb][ss];
   }
}

#if 0
//#ifdef MPEGA_STEREO
static void III_i_stereo_k_values(is_pos,io,i,k)
double io;
int is_pos,i;
double k[2][576];
          
{
   if(is_pos == 0)
   { 
      k[0][i] = 1;
      k[1][i] = 1;
   }
   else if ((is_pos % 2) == 1)
   {
      k[0][i] = pow(io,(double)((is_pos + 1)/2));
      k[1][i] = 1;
   }
   else
   {
      k[0][i] = 1;
      k[1][i] = pow(io,(double)(is_pos/2));
   }
}


void III_stereo(xr, lr, scalefac, gr_info, fr_ps)
double xr[2][SBLIMIT][SSLIMIT];
double lr[2][SBLIMIT][SSLIMIT];
III_scalefactor_t *scalefac;
gr_info_t *gr_info;
frame_params_t *fr_ps;
{
   int sfreq;
   int stereo = fr_ps->stereo;
   int ms_stereo = (fr_ps->header.mode == MPG_MD_JOINT_STEREO) &&
                   (fr_ps->header.mode_ext & 0x2); 
   int i_stereo = (fr_ps->header.mode == MPG_MD_JOINT_STEREO) &&
                  (fr_ps->header.mode_ext & 0x1);
   int js_bound;  /* frequency line that marks the beggining of the zero part */  
   int sfb,next_sfb_boundary;
   int i,j,sb,ss,ch;
   short is_pos[SBLIMIT*SSLIMIT]; 
   double is_ratio[SBLIMIT*SSLIMIT];
   double io;
   double k[2][SBLIMIT*SSLIMIT];

   int lsf	= (fr_ps->header.version == MPEG_PHASE2_LSF);

    if(  (gr_info->scalefac_compress % 2) == 1)
     {
       io = (double)0.707106781188;
     }
     else
     {
       io = (double)0.840896415256;
     }


   sfreq=fr_ps->header.sfreq_index + (fr_ps->header.version * 3);

  
   /* intialization */
   for ( i=0; i<SBLIMIT*SSLIMIT; i++ )
      is_pos[i] = 7;

   if ((stereo == 2) && i_stereo )
   {  if (gr_info->window_switching_flag && (gr_info->block_type == 2))
      {  if( gr_info->mixed_block_flag )
         {  int max_sfb = 0;

            for ( j=0; j<3; j++ )
            {  int sfbcnt;
               sfbcnt = 2;
               for( sfb=12; sfb >=3; sfb-- )
               {  int lines;
                  lines = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
                  i = 3*sfBandIndex[sfreq].s[sfb] + (j+1) * lines - 1;
                  while ( lines > 0 )
                  {  if ( xr[1][i/SSLIMIT][i%SSLIMIT] != 0.0 )
                     {  sfbcnt = sfb;
                        sfb = -10;
                        lines = -10;
                     }
                     lines--;
                     i--;
                  }
               }
               sfb = sfbcnt + 1;

               if ( sfb > max_sfb )
                  max_sfb = sfb;

               while( sfb<12 )
               {  sb = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
                  i = 3*sfBandIndex[sfreq].s[sfb] + j * sb;
                  for ( ; sb > 0; sb--)
                  {  is_pos[i] = (*scalefac)[1].s[j][sfb];
                     if ( is_pos[i] != 7 )
                         if( lsf )
                         {
                              III_i_stereo_k_values(is_pos[i],io,i,k);
                         }
                         else
                         {
                             is_ratio[i] = tan((double)is_pos[i] * (PI / 12));
                         }
                     i++;
                  }
                  sfb++;
               }

               sb = sfBandIndex[sfreq].s[12]-sfBandIndex[sfreq].s[11];
               sfb = 3*sfBandIndex[sfreq].s[11] + j * sb;
               sb = sfBandIndex[sfreq].s[13]-sfBandIndex[sfreq].s[12];

               i = 3*sfBandIndex[sfreq].s[11] + j * sb;
               for ( ; sb > 0; sb-- )
               {  is_pos[i] = is_pos[sfb];
                  is_ratio[i] = is_ratio[sfb];
                  k[0][i] = k[0][sfb];
                  k[1][i] = k[1][sfb];
                  i++;
               }
             }
             if ( max_sfb <= 3 )
             {  i = 2;
                ss = 17;
                sb = -1;
                while ( i >= 0 )
                {  if ( xr[1][i][ss] != 0.0 )
                   {  sb = i*18+ss;
                      i = -1;
                   } else
                   {  ss--;
                      if ( ss < 0 )
                      {  i--;
                         ss = 17;
                      }
                   }
                }
                i = 0;
                while ( sfBandIndex[sfreq].l[i] <= sb )
                   i++;
                sfb = i;
                i = sfBandIndex[sfreq].l[i];
                for ( ; sfb<8; sfb++ )
                {  sb = sfBandIndex[sfreq].l[sfb+1]-sfBandIndex[sfreq].l[sfb];
                   for ( ; sb > 0; sb--)
                   {  is_pos[i] = (*scalefac)[1].l[sfb];
                      if ( is_pos[i] != 7 )
                         if ( lsf )
                         {
                              III_i_stereo_k_values(is_pos[i],io,i,k);
                         }
                         else
                         {
                             is_ratio[i] = tan((double)is_pos[i] * (PI / 12));
                         }
                      i++;
                   }
                }
            }
         } else
         {  for ( j=0; j<3; j++ )
            {  int sfbcnt;
               sfbcnt = -1;
               for( sfb=12; sfb >=0; sfb-- )
               {  int lines;
                  lines = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
                  i = 3*sfBandIndex[sfreq].s[sfb] + (j+1) * lines - 1;
                  while ( lines > 0 )
                  {  if ( xr[1][i/SSLIMIT][i%SSLIMIT] != 0.0 )
                     {  sfbcnt = sfb;
                        sfb = -10;
                        lines = -10;
                     }
                     lines--;
                     i--;
                  }
               }
               sfb = sfbcnt + 1;
               while( sfb<12 )
               {  sb = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
                  i = 3*sfBandIndex[sfreq].s[sfb] + j * sb;
                  for ( ; sb > 0; sb--)
                  {  is_pos[i] = (*scalefac)[1].s[j][sfb];
                     if ( is_pos[i] != 7 )
                         if( lsf )
                         {
                              III_i_stereo_k_values(is_pos[i],io,i,k);
                         }
                         else
                         {
                             is_ratio[i] = tan( (double)is_pos[i] * (PI / 12));
                         }
                     i++;
                  }
                  sfb++;
               }

               sb = sfBandIndex[sfreq].s[12]-sfBandIndex[sfreq].s[11];
               sfb = 3*sfBandIndex[sfreq].s[11] + j * sb;
               sb = sfBandIndex[sfreq].s[13]-sfBandIndex[sfreq].s[12];

               i = 3*sfBandIndex[sfreq].s[11] + j * sb;
               for ( ; sb > 0; sb-- )
               {  is_pos[i] = is_pos[sfb];
                  is_ratio[i] = is_ratio[sfb];
                  k[0][i] = k[0][sfb];
                  k[1][i] = k[1][sfb];
                  i++;
               }
            }
         }
      } else
      {  i = 31;
         ss = 17;
         sb = 0;
         while ( i >= 0 )
         {  if ( xr[1][i][ss] != 0.0 )
            {  sb = i*18+ss;
               i = -1;
            } else
            {  ss--;
               if ( ss < 0 )
               {  i--;
                  ss = 17;
               }
            }
         }
         i = 0;
         while ( sfBandIndex[sfreq].l[i] <= sb )
            i++;
         sfb = i;
         i = sfBandIndex[sfreq].l[i];
         for ( ; sfb<21; sfb++ )
         {  sb = sfBandIndex[sfreq].l[sfb+1] - sfBandIndex[sfreq].l[sfb];
            for ( ; sb > 0; sb--)
            {  is_pos[i] = (*scalefac)[1].l[sfb];
               if ( is_pos[i] != 7 )
                     if( lsf )
                     {
                           III_i_stereo_k_values(is_pos[i],io,i,k);
                     }
                     else
                     {
                          is_ratio[i] = tan((double)is_pos[i] * (PI / 12));
                     }
               i++;
            }
         }
         sfb = sfBandIndex[sfreq].l[20];
         for ( sb = 576 - sfBandIndex[sfreq].l[21]; sb > 0; sb-- )
         {  is_pos[i] = is_pos[sfb];
            is_ratio[i] = is_ratio[sfb];
            k[0][i] = k[0][sfb];
            k[1][i] = k[1][sfb];
            i++;
         }
      }
   }

   for(ch=0;ch<2;ch++)
      for(sb=0;sb<SBLIMIT;sb++)
         for(ss=0;ss<SSLIMIT;ss++) 
            lr[ch][sb][ss] = 0;

   if (stereo==2) 
      for(sb=0;sb<SBLIMIT;sb++)
         for(ss=0;ss<SSLIMIT;ss++) {
            i = (sb*18)+ss;
            if ( is_pos[i] == 7 ) {
               if ( ms_stereo ) {
                  lr[0][sb][ss] = (xr[0][sb][ss]+xr[1][sb][ss])/(double)1.41421356;
                  lr[1][sb][ss] = (xr[0][sb][ss]-xr[1][sb][ss])/(double)1.41421356;
               }
               else {
                  lr[0][sb][ss] = xr[0][sb][ss];
                  lr[1][sb][ss] = xr[1][sb][ss];
               }
            }
            else if (i_stereo ) {
                if ( lsf )
                {
                  lr[0][sb][ss] = xr[0][sb][ss] * k[0][i];
                  lr[1][sb][ss] = xr[0][sb][ss] * k[1][i]; 
                }
               else
                {
                  lr[0][sb][ss] = xr[0][sb][ss] * (is_ratio[i]/(1+is_ratio[i]));
                  lr[1][sb][ss] = xr[0][sb][ss] * (1/(1+is_ratio[i])); 
                }
            }
            else {
               fprintf(stderr,"Error in streo processing\n");
            }
         }
   else  /* mono , bypass xr[0][][] to lr[0][][]*/
      for(sb=0;sb<SBLIMIT;sb++)
         for(ss=0;ss<SSLIMIT;ss++)
            lr[0][sb][ss] = xr[0][sb][ss];

}

double Ci[8]={-0.6,-0.535,-0.33,-0.185,-0.095,-0.041,-0.0142,-0.0037};


void III_antialias(xr, hybridIn, gr_info, fr_ps)
double xr[SBLIMIT][SSLIMIT];    
double hybridIn[SBLIMIT][SSLIMIT];
gr_info_t *gr_info;             
frame_params_t *fr_ps;            
{
   static int    init = 1;
   static double ca[8],cs[8];
   double        bu,bd;  /* upper and lower butterfly inputs */
   int           ss,sb,sblim;

   if (init) {
      int i;
      double    sq;
      for (i=0;i<8;i++) {
         sq=sqrt(1.0+Ci[i]*Ci[i]);
         cs[i] = 1.0/sq;
         ca[i] = Ci[i]/sq;
      }
      init = 0;
   }
   
   /* clear all inputs */  
      
    for(sb=0;sb<SBLIMIT;sb++)
       for(ss=0;ss<SSLIMIT;ss++)
          hybridIn[sb][ss] = xr[sb][ss];

   if  (gr_info->window_switching_flag && (gr_info->block_type == 2) &&
       !gr_info->mixed_block_flag ) return;

   if ( gr_info->window_switching_flag && gr_info->mixed_block_flag &&
     (gr_info->block_type == 2))
      sblim = 1;
   else
      sblim = SBLIMIT-1;

   /* 31 alias-reduction operations between each pair of sub-bands */
   /* with 8 butterflies between each pair                         */

   for(sb=0;sb<sblim;sb++)   
      for(ss=0;ss<8;ss++) {      
         bu = xr[sb][17-ss];
         bd = xr[sb+1][ss];
         hybridIn[sb][17-ss] = (bu * cs[ss]) - (bd * ca[ss]);
         hybridIn[sb+1][ss] = (bd * cs[ss]) + (bu * ca[ss]);
         }  
}
#endif

void inv_mdct(in, out, block_type)
double in[18];
double out[36];
int block_type;
{
/*------------------------------------------------------------------*/
/*                                                                  */
/*    Function: Calculation of the inverse MDCT                     */
/*    In the case of short blocks the 3 output vectors are already  */
/*    overlapped and added in this modul.                           */
/*                                                                  */
/*    New layer3                                                    */
/*                                                                  */
/*------------------------------------------------------------------*/

int     k,i,m,N,p;
double  tmp[12],sum;
static  double  win[4][36];
static  int init=0;
static  double COS[4*36];

    if(init==0){

    /* type 0 */
      for(i=0;i<36;i++)
         win[0][i] = sin( PI/36 *(i+0.5) );

    /* type 1*/
      for(i=0;i<18;i++)
         win[1][i] = sin( PI/36 *(i+0.5) );
      for(i=18;i<24;i++)
         win[1][i] = 1.0;
      for(i=24;i<30;i++)
         win[1][i] = sin( PI/12 *(i+0.5-18) );
      for(i=30;i<36;i++)
         win[1][i] = 0.0;

    /* type 3*/
      for(i=0;i<6;i++)
         win[3][i] = 0.0;
      for(i=6;i<12;i++)
         win[3][i] = sin( PI/12 *(i+0.5-6) );
      for(i=12;i<18;i++)
         win[3][i] =1.0;
      for(i=18;i<36;i++)
         win[3][i] = sin( PI/36*(i+0.5) );

    /* type 2*/
      for(i=0;i<12;i++)
         win[2][i] = sin( PI/12*(i+0.5) ) ;
      for(i=12;i<36;i++)
         win[2][i] = 0.0 ;

      for (i=0; i<4*36; i++)
         COS[i] = cos(PI/(2*36) * i);

      init++;
    }

    for(i=0;i<36;i++)
       out[i]=0;

    if(block_type == 2){
       N=12;
       for(i=0;i<3;i++){
          for(p= 0;p<N;p++){
             sum = 0.0;
             for(m=0;m<N/2;m++)
                sum += in[i+3*m] * cos( PI/(2*N)*(2*p+1+N/2)*(2*m+1) );
             tmp[p] = sum * win[block_type][p] ;
          }
          for(p=0;p<N;p++)
             out[6*i+p+6] += tmp[p];
       }
    }
    else{
      N=36;
      for(p= 0;p<N;p++){
         sum = 0.0;
         for(m=0;m<N/2;m++)
           sum += in[m] * COS[((2*p+1+N/2)*(2*m+1))%(4*36)];
         out[p] = sum * win[block_type][p];
      }
    }
}

void III_hybrid(fsIn, tsOut ,sb, ch, gr_info, fr_ps)
double fsIn[SSLIMIT];   /* freq samples per subband in */
double tsOut[SSLIMIT];  /* time samples per subband out */
int sb, ch;
gr_info_t *gr_info;             
frame_params_t *fr_ps;            
{
   int ss;
   double rawout[36];
   static double prevblck[2][SBLIMIT][SSLIMIT];
   static int init = 1;
   int bt;

   if (init) {
      int i,j,k;
      
      for(i=0;i<2;i++)
         for(j=0;j<SBLIMIT;j++)
            for(k=0;k<SSLIMIT;k++)
               prevblck[i][j][k]=0.0;
      init = 0;
   }

   bt = (gr_info->window_switching_flag && gr_info->mixed_block_flag &&
          (sb < 2)) ? 0 : gr_info->block_type; 

   inv_mdct( fsIn, rawout, bt);

   /* overlap addition */
   for(ss=0; ss<SSLIMIT; ss++) {
      tsOut[ss] = rawout[ss] + prevblck[ch][sb][ss];
      prevblck[ch][sb][ss] = rawout[ss+18];
   }
}

