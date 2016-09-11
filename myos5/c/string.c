/* string.c */

void memset(void *dest, unsigned char val, unsigned int len)
{
	unsigned int i=0;
	while (i<len) {
		*((unsigned char*)dest++)=val;
		i++;
	}
}

void memcpy(void *dest, void *src, unsigned int len)
{
	unsigned int i=0;
	while (i<len){
		*((unsigned char*)dest++)=*((unsigned char*)src++);
		i++;
	}
}

