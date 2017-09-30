#include "apue.h"
#include <fcntl.h>

static void set_fl(int fd, int flags);

int
main(void)
{
	int fdout=STDOUT_FILENO, n, oflag;
	char buf[BUFFSIZE];
	
#ifdef OUTFILE
	oflag = O_WRONLY | O_CREAT;
	#ifdef OSYNC
		oflag |= O_SYNC;
	#endif
	if((fdout=open(OUTFILE,oflag, FILE_MODE)) == -1)
		perror("open error"), exit(-1);
#else

	#ifdef OSYNC
		set_fl(STDOUT_FILENO, O_SYNC);
	#endif
#endif

	while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0)
	{
		if(write(fdout,buf,n) != n)
			perror("write error"), exit(-1);
	}
	if(n<0)
		perror("read error"),exit(-1);

#ifdef FSYNC
	fsync(STDOUT_FILENO);
#endif	

	exit(0);
}



static void set_fl(int fd, int flags)
{
	int val;
	if((val=fcntl(fd, F_GETFL, 0)) < 0)
	   perror("fcntl F_GETFL error"), exit(-1);
	val |= flags;

	if(fcntl(fd, F_SETFL, val) < 0)
		perror("fcntl F_SETFL error"),exit(-1);
}
