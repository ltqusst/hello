#include "apue.h"
#include <sys/wait.h>

static void sig_int(int);

int main(void)
{
	char 	buf[MAXLINE];
	pid_t 	pid;
	int status, n;
	int fd_dup[3];

	strcpy(buf, "a message to STDOUT_FILENO\n");

	fd_dup[0] = dup(STDIN_FILENO);
	fd_dup[1] = dup(STDOUT_FILENO);
	fd_dup[2] = dup(STDERR_FILENO);

	printf("uid=%d, gid=%d, pid=%ld, ppid=%ld\n", getuid(),getgid(),(long)getpid(), (long)getppid());
    printf("FOPEN_MAX=%d, _SC_OPEN_MAX=%ld\n",	FOPEN_MAX, sysconf(_SC_OPEN_MAX));
	printf("_SC_ARG_MAX=%ld,_SC_CLK_TCK=%ld\n", sysconf(_SC_ARG_MAX),sysconf(_SC_CLK_TCK));

	if(signal(SIGINT, sig_int) == SIG_ERR)
		perror("signal error"), exit(-1);

	printf("%% ");
	while(fgets(buf,MAXLINE,stdin) != NULL){
		
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		printf("[PID:%ld %s]\n", (long)getpid(), buf);

		if((pid=fork())<0){
			perror("fork error");
			exit(-1);
		}else if (pid == 0){

			/*
			if(dup2(fd_dup[0], STDIN_FILENO) != STDIN_FILENO)
				perror("dup2(0) error"), exit(-1);
			if(dup2(fd_dup[1], STDOUT_FILENO) != STDOUT_FILENO)
				perror("dup2(1) error"), exit(-1);
			if(dup2(fd_dup[2], STDERR_FILENO) != STDERR_FILENO)
				perror("dup2(2) error"), exit(-1);
*/
			while(1)
			{
				printf("PID=%ld, PPID=%ld\n", (long)getpid(), (long)getppid());
				sleep(2);

				if((n=read(STDIN_FILENO, buf, sizeof(buf))) > 0)
				{
					buf[n] = 0;
					printf("read from STDIN_FILENO: %s\n", buf);
				}
			}

			execlp(buf,buf,(char*)0);
			perror("execlp error");
			exit(-1);
		}

		/*parent*/
		//if((pid=waitpid(pid,&status,0))<0)
		//	perror("waitpid error"), exit(-1);
		exit(0);

		printf("%% ");
	}

	exit(0);
}

void sig_int(int signo)
{
	printf("interrupt %d\n%% ", signo);
}

