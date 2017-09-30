#include "apue.h"
#include <sys/wait.h>
#include <fcntl.h>


char * myname;

int cat(int fd)
{
	char buf[4096];
	int n;
	int cnt = 0;

	// without this lseek(), one process will read exclusively
	//lseek(fd, 0, SEEK_SET);

	while((n=read(fd, buf, sizeof(buf))) > 0)
	{
		write(STDOUT_FILENO, myname, strlen(myname));

		if(write(STDOUT_FILENO, buf, n) != n)
			perror("write error"), exit(-1);

		cnt += n;
	}

	if(n < 0){
		sprintf(buf, "%s read error", myname);
		perror(buf);
	}

	// close one fd duplicate will not cause kernel to
	// release internal item in file table. there is
	// some reference counting mechanism.

	if(close(fd)){
		sprintf(buf, "%s close(fd) error", myname);
		perror(buf);
	}
	return cnt;
}

int main(int argc, char * argv[])
{
	char 	buf[MAXLINE];
	pid_t 	pid;
	int status, n;
	int fd;

	printf("uid=%d, gid=%d, pid=%ld, ppid=%ld\n", getuid(),getgid(),(long)getpid(), (long)getppid());
    //printf("FOPEN_MAX=%d, _SC_OPEN_MAX=%ld\n",	FOPEN_MAX, sysconf(_SC_OPEN_MAX));
	//printf("_SC_ARG_MAX=%ld,_SC_CLK_TCK=%ld\n", sysconf(_SC_ARG_MAX),sysconf(_SC_CLK_TCK));

	if((fd = open(argv[1], O_RDONLY)) == -1)
		perror("open error"), exit(-1);

	if((pid=fork())<0){
		perror("fork error");
		exit(-1);
	}else if (pid == 0){
		myname = " child:";
		fprintf(stderr,"%s cat from fd%d start\n", myname, fd);
		n = cat(fd);
	}else{
		myname = "parent:";
		sleep(2);
		fprintf(stderr,"%s cat from fd%d start\n", myname, fd);
		n = cat(fd);
	}

	/*parent*/
	//if((pid=waitpid(pid,&status,0))<0)
	//	perror("waitpid error"), exit(-1);

	fprintf(stderr,"%s cat over, %d bytes read%s\n", myname, n,
			n>0?"":"\n\t***(internal file pointer is shared, the other process must have read all.)***");

	if(pid)
		sleep(10);

	fprintf(stderr,"%s exit\n", myname);
	exit(0);
}
