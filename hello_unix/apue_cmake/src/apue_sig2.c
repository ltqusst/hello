/*
 * apue_sig2.c
 *
 *  Created on: Sep 6, 2017
 *      Author: tingqian
 */


#include "apue.h"

int
main(int argc, char * argv[])
{
	int i;
	for(i=0;i<10000;i++)
	{
		sleep(1);
		printf("%s: %d\n", argv[0], i);
	}
	exit(0);
}
