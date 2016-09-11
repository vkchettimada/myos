/* io.c */
#include <asm/io.h>

__inline unsigned char inpb (unsigned short int port)
{
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

__inline unsigned short int inpw (unsigned short int port)
{
  unsigned short _v;

  __asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

__inline unsigned int inpl (unsigned short int port)
{
  unsigned int _v;

  __asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

__inline void inpsb(unsigned short port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ ("rep; insb" : "+D" (addr), "+c" (count) : "d" (port));
}

__inline void inpsw(unsigned short port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ ("rep; insw" : "+D" (addr), "+c" (count) : "d" (port));
}

__inline void inpsl(unsigned short port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ ("rep; insl" : "+D" (addr), "+c" (count) : "d" (port));
}

__inline void outpb (unsigned char value, unsigned short int port)
{
  __asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (port));
}

__inline void outpw (unsigned short int value, unsigned short int port)
{
  __asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd" (port));
}

__inline void outpl (unsigned int value, unsigned short int port)
{
  __asm__ __volatile__ ("outl %0,%w1": :"a" (value), "Nd" (port));
}

__inline void outpsb(const void *addr, unsigned long count, unsigned short port)
{
	__asm__ __volatile__ ("rep; outsb" : "+S" (addr), "+c" (count) : "d" (port));
}

__inline void outpsw(const void *addr, unsigned long count, unsigned short port)
{
	__asm__ __volatile__ ("rep; outsw" : "+S" (addr), "+c" (count) : "d" (port));
}

__inline void outpsl(const void *addr, unsigned long count, unsigned short port)
{
	__asm__ __volatile__ ("rep; outsl" : "+S" (addr), "+c" (count) : "d" (port));
}

__inline void writel(unsigned int value, unsigned char *mem)
{
	unsigned int *p= (unsigned int*) mem;
	*p=value;
}

__inline unsigned int readl(unsigned char *mem)
{
	return(*(unsigned int*)mem);
}

