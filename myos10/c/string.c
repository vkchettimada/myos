/* string.c */

void memset(void *dest, unsigned char val, unsigned int len)
{
	while (len--) {
		*((unsigned char*)dest++)=val;
	}
}

void memcpy(void *dest, void *src, unsigned int len)
{
	while (len--){
		*((unsigned char*)dest++)=*((unsigned char*)src++);
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

