/* io.h */

#ifndef _IO_H_
#define _IO_H_
unsigned char inpb (unsigned short int port);
unsigned short int inpw (unsigned short int port);
unsigned int inpl (unsigned short int port);
void inpsb(unsigned short port, const void *addr, unsigned long count);
void inpsw(unsigned short port, const void *addr, unsigned long count);
void inpsl(unsigned short port, const void *addr, unsigned long count);
void outpb (unsigned char value, unsigned short int port);
void outpw (unsigned short int value, unsigned short int port);
void outpl (unsigned int value, unsigned short int port);
void outpsb(const void *addr, unsigned long count, unsigned short port);
void outpsw(const void *addr, unsigned long count, unsigned short port);
void outpsl(const void *addr, unsigned long count, unsigned short port);
void writel(unsigned int value, unsigned char *mem);
unsigned int readl(unsigned char *mem);
#endif

