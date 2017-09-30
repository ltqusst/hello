/*
 * apue_term1.c
 *
 *  Created on: Sep 19, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <pthread.h>
#include <termios.h>

void * start(void*arg)
{
	struct timespec spec, remain;
	spec.tv_sec = 3;
	spec.tv_nsec = 0;

	while(1){
		nanosleep(&spec, &remain);
		//printf(".\n");
		//tcflush(STDIN_FILENO, TCIFLUSH);
	}
}

int main(int argc, char * argv[])
{
	int n;
	char buf[10];
	pthread_t pid_t;

	pthread_create(&pid_t, NULL, start, NULL);

	while((n=read(STDIN_FILENO, buf, sizeof(buf))) > 0){
		fprintf(stderr,"{%d bytes read}", n);
		write(STDOUT_FILENO, buf, n);
	}
	if(n == 0)
	fprintf(stderr,"\n Input end \n Joininig thread...\n");
	pthread_join(pid_t, NULL);
	exit(0);
}
