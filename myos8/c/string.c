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

int strcmp(char *s1, char *s2)
{
	while (*s1 && *s2) {
		if (*s1 < *s2)
			return(-1);
		if (*s1 > *s2)
			return(1);
		s1++;
		s2++;
	}
	if (*s1 == 0 && *s2 != 0)
		return(-1);
	if (*s1 != 0 && *s2 == 0)
		return(1);
	return(0);
}

