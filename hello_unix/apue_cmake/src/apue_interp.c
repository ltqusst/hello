#include "apue.h"
#include <sys/wait.h>

int main(int argc, char * argv[])
{
	pid_t pid;
	int status;

	if((pid = fork()) < 0){
		perror("fork error"),exit(-1);
	}else if(pid == 0){  /* child */
		if(execl(argv[1], argv[2], argv[3], argv[4], (char*)0) < 0)
			perror("execl error"), exit(-1);
	}

	if(waitpid(pid, &status, 0) < 0)
		perror("waitpid error"), exit(-1);
	else
		if(WEXITSTATUS(status)){
			fprintf(stderr,"child exit with code %d\n", WEXITSTATUS(status));
			exit(WEXITSTATUS(status));
		}

	exit(0);
}
