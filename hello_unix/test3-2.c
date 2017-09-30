#include "apue.h"
#include <fcntl.h>

char buf1[]="abcdefghij";
char buf2[]="ABCDEFGHIJ";

int main(void)
{
	int fd;

	if((fd=creat("file.hole",FILE_MODE)) < 0)
		perror("create error"), exit(errno);

	if(write(fd, buf1, 10) != 10)
		perror("buf1 write error"), exit(-1);

	if(lseek(fd, 16384, SEEK_SET) == -1)
		perror("lseek error"), exit(-1);

	if(write(fd, buf2, 10)!= 10)
		perror("buf2 write error"), exit(-1);

	exit(0);
}

