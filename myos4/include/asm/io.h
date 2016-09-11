/* io.h */

#ifndef _IO_H_
#define _IO_H_
unsigned char inpb (unsigned short int port);
unsigned short int inpw (unsigned short int port);
unsigned int inpl (unsigned short int port);
void outpb (unsigned char value, unsigned short int port);
void outpw (unsigned short int value, unsigned short int port);
void outpl (unsigned int value, unsigned short int port);
#endif

