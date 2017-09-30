#include "apue.h"
#include <sys/wait.h>

static void sig_int(int);

int main(void)
{
	char 	buf[MAXLINE];
	pid_t 	pid;
	int status;

	printf("uid=%d, gid=%d, pid=%ld\n", getuid(),getgid(),(long)getpid());
    printf("FOPEN_MAX=%d, _SC_OPEN_MAX=%ld\n",	FOPEN_MAX, sysconf(_SC_OPEN_MAX));
	printf("_SC_ARG_MAX=%ld,_SC_CLK_TCK=%ld\n", sysconf(_SC_ARG_MAX),sysconf(_SC_CLK_TCK));

	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_puts(errno, "signal error"), exit(errno);

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin) != NULL){
		
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if((pid=fork())<0){
			err_puts(errno,"fork error");
			exit(errno);
		}else if (pid == 0){
			execlp(buf,buf,(char*)0);
			err_puts(errno,"couldn't execute : %s", buf);
			exit(127);
		}

		/*parent*/
		if((pid=waitpid(pid,&status,0))<0)
			err_puts(errno,"waitpid error"), exit(errno);

		printf("%% ");
	   		
	}

	exit(0);
}

void sig_int(int signo)
{
	printf("interrupt %d\n%% ", signo);
}

