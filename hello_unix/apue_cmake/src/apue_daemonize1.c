/*
 * apue_daemonize1.c
 *
 *  Created on: Sep 6, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <syslog.h>
#include "apue_utils.h"

static void sig_term(int signo);

int
main(int argc, char * argv[])
{
	struct sigaction sa;
	int i;

	/* Become a daemon first */
	daemonize(argv[0], 0);

	/* Interact with user through signal & syslog */
	sa.sa_handler = sig_term;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGTERM, &sa, NULL) < 0)
		syslog(LOG_ERR, "install handler for SIGTERM error: %m\n");

	for(i=0;i<120;i++){
		sleep(1);
		syslog(LOG_INFO, "UID=%ld Daemon check in %s(%ld)",
				(long)getuid(), argv[0], (long)getpid());
	}
	syslog(LOG_INFO, "Daemon exit");
	exit(0);
}


static void sig_term(int signo)
{
	syslog(LOG_INFO, "SIGTERM(%d) received, exit.", signo);
	exit(signo);
}



