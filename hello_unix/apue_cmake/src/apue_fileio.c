#include "apue.h"
#include <stdio.h>

#define BUFMAXSIZE 524288

char buf[BUFMAXSIZE];

int main(int argc, char *argv[])
{
	if(argc != 2)
		fprintf(stderr, "Usage: %s bufsize", argv[0]), exit(1);

	int bufsz = atoi(argv[1]);
	int n;
	while((n=read(STDIN_FILENO, buf, bufsz)) > 0)
		if(write(STDOUT_FILENO, buf, n) !=n )
			perror("write error"), exit(-1);

	if(n<0)
		perror("read error"),exit(-1);

	exit(0);
}
