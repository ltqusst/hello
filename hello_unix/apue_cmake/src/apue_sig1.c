/*
 * apue_sig1.c
 *
 *  Created on: Sep 6, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <math.h>
#include <setjmp.h>
#include <signal.h>

unsigned int sleep2(unsigned int);
static void sig_int(int);

int
main(int argc, char * argv[])
{
	unsigned int unslept;

	int timeout = 5;
	int opt;
	while((opt = getopt(argc,argv,"t:")) > 0)
	{
		switch(opt)
		{
		case 't':
			timeout = strtol(optarg, NULL, 10);
			break;
		}
	}

	if(signal(SIGINT, sig_int) == SIG_ERR)
		perror("signal(SIGINT) error"), exit(1);

	printf("calling sleep2(timeout=%d) \n", timeout);

	unslept = sleep2(timeout);
	printf("sleep2 returned: %u\n", unslept);
	exit(0);
}

static void sig_int(int signo)
{
	int 			i,j;
	volatile int 	k;

	printf("\nsig_int starting\n");

	for(i=0;i<30000;i++)
		for(j=0;j<4000;j++)
			k+=i * sqrt(j);

	printf("sig_int finished with k=%d\n", k);
}



static jmp_buf env_alrm;
static void
sig_alrm(int signo)
{
	longjmp(env_alrm,1);
}
unsigned int sleep2(unsigned int seconds)
{
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		return seconds;

	if(setjmp(env_alrm) == 0){
		alarm(seconds);
		pause();
	}

	/*any other signal also can cause pause() exit
	 *earlier, so sleep2 may exit earlier too.
	 *in that case we need return un-slept time to caller */

	return alarm(0);
}

