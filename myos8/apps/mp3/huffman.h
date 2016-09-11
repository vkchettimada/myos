/* huffman.h */

#define HUFFBITS unsigned long int
#define HTN	34
#define MXOFF	250
 
/* Information needed to recover .table lines*/
typedef struct 
{
    int		   treelen;	/* 3rd col*/
    int		   xlen;	/* 4th col*/
    int		   ylen;	/* 5th col*/
    int		   linbits;	/* 6th col*/
    /* Information for .treedata or .reference lines*/
    int		   ref;		/* .reference table, or .treedata if -1*/
    unsigned char *data;	/* ptr to treedata array*/
} huffinfo_t;

typedef struct 
{
  char tablename[3];	/*string, containing table_description	*/
  unsigned int xlen; 	/*max. x-index+			      	*/ 
  unsigned int ylen;	/*max. y-index+				*/
  unsigned int linbits; /*number of linbits			*/
  unsigned int linmax;	/*max number to be stored in linbits	*/
  int ref;		/*a positive value indicates a reference*/
  HUFFBITS *table;	/*pointer to array[xlen][ylen]		*/
  unsigned char *hlen;	/*pointer to array[xlen][ylen]		*/
  unsigned char(*val)[2];/*decoder tree				*/ 
  unsigned int treelen;	/*length of decoder tree		*/
}huffcodetab_t;

extern huffcodetab_t ht[HTN];/* global memory block		*/
				/* array of all huffcodtable headers	*/
				/* 0..31 Huffman code table 0..31	*/
				/* 32,33 count1-tables			*/
