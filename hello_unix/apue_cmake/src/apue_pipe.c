/*
 * apue_pipe.c
 *
 *  Created on: Sep 11, 2017
 *      Author: tingqian
 */

#include "apue.h"


char buf_out[]=
		"this is line1\n"
		"this is line2\n"
		"this is line3\n"
		"the end";


const char * stat_type(struct stat *pst);


int main(void)
{
	struct stat st;
	int n;
	int fd[2];
	pid_t pid;
	char buf[1024];

	if(pipe(fd) < 0)
		perror("pipe error"), exit(1);

	if((pid=fork()) < 0){
		perror("fork error");
		exit(1);
	}else if(pid > 0){
		close(fd[0]);
		n = write(fd[1], buf_out, sizeof(buf_out));

		if(n != sizeof(buf_out))
			perror("write error"), exit(1);

		fprintf(stderr,"parent exit after write(%d) bytes\n", n);
	}else{

		close(fd[1]);

		sleep(2);

		if(fstat(fd[0], &st) < 0)
			perror("fstat error"),exit(1);

		printf("fd[0]: type=%s, size=%d bytes\n", stat_type(&st), st.st_size);

		while((n=read(fd[0], buf, sizeof(buf))) > 0){
			fprintf(stderr,"fd[0] read %3d bytes:{",n);

			if(write(STDOUT_FILENO, buf, n) != n)
				perror("write error"),exit(1);

			fprintf(stderr,"}\n");
		}


		if(n < 0)
			perror("read error"),exit(1);

		fprintf(stderr,"child exit\n");
	}

	exit(0);
}


const char * stat_type(struct stat *pst)
{
	const char * ptr = '-';
	if(S_ISREG(pst->st_mode)) 		ptr="regular file";
	else if(S_ISDIR(pst->st_mode)) 	ptr="dir";
	else if(S_ISCHR(pst->st_mode)) 	ptr="char";
	else if(S_ISBLK(pst->st_mode)) 	ptr="block";
	else if(S_ISFIFO(pst->st_mode)) ptr="fifo/pipe";
	else if(S_ISLNK(pst->st_mode)) 	ptr="link(symbolic)";
	else if(S_ISSOCK(pst->st_mode)) ptr="socket";

	return ptr;
}



