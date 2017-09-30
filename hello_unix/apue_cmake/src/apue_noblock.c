/*
 * apue_noblock.c
 *
 *  Created on: Sep 9, 2017
 *      Author: tingqian
 */
#include <stdio.h>
#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "apue_utils.h"

char buf[500000];

int
main(int argc, char *argv[])
{
	int ntowrite, nwrite;
	char * ptr;

	if(argc < 2 || strlen(argv[1]) != 2)
		fprintf(stderr,
				"Usage: %s stdin/out_nonblock(y/n)(y/n)\n"
				"for exmaple:\n"
				"\t'%s yy' set both stdin/out to nonblock mode\n"
				"\t'%s yn' set only stdin to nonblock mode\n",
				argv[0],argv[0],argv[0]), exit(-1);

	if(argv[1][0] == 'y') fl_set(STDIN_FILENO, O_NONBLOCK);
	if(argv[1][1] == 'y') fl_set(STDOUT_FILENO, O_NONBLOCK);

	errno = 0;
	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes, errno=%d(%s)\n", ntowrite, errno, strerror(errno));

	ptr = buf;
	while(ntowrite > 0){
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);

		if(errno)
			errno;
			//fprintf(stderr, "nwrite=%d, errno=%d(%s)\n", nwrite, errno, strerror(errno));
		else
			fprintf(stderr,"nwrite = %d\n", nwrite);

		if(nwrite > 0)
			ptr += nwrite, ntowrite-= nwrite;
	}

	fl_clr(STDIN_FILENO, O_NONBLOCK);
	fl_clr(STDOUT_FILENO, O_NONBLOCK);
	exit(0);
}



