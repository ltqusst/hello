#include "apue.h"

#define BUFFSIZE 4096

int
main(void)
{
	int n;
	char buf[BUFFSIZE];

	fprintf(stderr,	"1. EOF=%d, so its not a ASCII code\n", EOF);
	fprintf(stderr,	"2. when STDIN is a ternimal rather than a binary file, \n"
		   			"   only ASCII code can be sent and some control code \n"
		   			"   like Ctrl+D was used to mean end-of-file\n");
	fprintf(stderr,	"3. terminal will cache a line until \\n was met, and \n"
		   			"    before that, any editing of the line is possible w/o\n"
		   			"    causing the editing character being included in the file content\n");
	fprintf(stderr,"----------------------\n");

	while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0)
	{
		fprintf(stderr,"[%d bytes were read]:",n);
		if(write(STDOUT_FILENO,buf,n) != n)
		{
			err_puts(errno,"write error!\n");
			exit(errno);
		}
	}
	if(n<0)
		err_puts(errno,"read error"),exit(errno);
	exit(0);
}

