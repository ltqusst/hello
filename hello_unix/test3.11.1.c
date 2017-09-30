#include "apue.h"
#include <fcntl.h>

char buf1[]="abcdefghi\n";

int main(void)
{
	int fd;
	off_t o1,o2;

	if((fd=open("ftemp", OFLAG, FILE_MODE)) < 0)
		perror("open failed"), exit(-1);

	if(LSEEK > 0)
	{
		if(lseek(fd, LSEEK, SEEK_SET) == -1)
			perror("lseek error"), exit(-1);
	}
	
	o1 = lseek(fd,0,SEEK_CUR);

	if(write(fd, buf1, 10) != 10)
		perror("write failed"), exit(-1);
	
	o2 = lseek(fd,0,SEEK_CUR);

	printf("offset before/after write: %ld, %ld\n", (long)o1, (long)o2);

	exit(0);
}


