#include "apue.h"

int main(void)
{
	if(lseek(STDIN_FILENO,0,SEEK_CUR) == -1)
		perror("lseek error"), exit(errno);
	else
		printf("SEEK OK\n");
	exit(0);
}

