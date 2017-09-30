/*
 * apue_dup.c
 *
 *  Created on: Sep 10, 2017
 *      Author: tingqian
 */

#include "apue.h"
#include <dirent.h>
#include <fcntl.h>

void list_dir(const char * dir_name, const char * prefix)
{
	DIR * pdir;
	struct dirent * ent;
	struct stat st;
	char strpath[1024];
	char symlink[1024];
	int n;

	pdir = opendir(dir_name);

	while((ent = readdir(pdir)) != NULL)
	{
		sprintf(strpath,"%s/%s", dir_name, ent->d_name);

		if(lstat(strpath, &st) == -1)
			perror("stat error"), exit(1);

		if(S_ISLNK(st.st_mode))
		{
			if((n = readlink(strpath, symlink, sizeof(symlink))) == -1)
				perror("readlink error"), exit(1);

			if(n > 0)
				symlink[n] = 0;
			else
				strcpy(symlink,"?");
			printf("%s%s -> %s\n",prefix, strpath, symlink);
		}
		else
			printf("%s\n", prefix,strpath);
	}

	closedir(pdir);
}

int
main(void)
{
	int fdin;
	int n;
	char buf[1024];

	struct stat st;

	if(fstat(STDIN_FILENO, &st) == -1)
		perror("fstat error"), exit(1);

	if(S_ISREG(st.st_mode)) printf("STDIN is a regular file:ino=%d\n", st.st_ino);
	else if(S_ISCHR(st.st_mode)) printf("STDIN is a char device:ino=%d\n", st.st_ino);
	else printf("??? \n");

	printf("-------list dir (/dev/fd)---------\n");;
	list_dir("/dev/fd","\t");


	if((n=read(STDIN_FILENO, buf, 10)) > 0)
	{
		fprintf(stderr,"[read() %d bytes]:{", n);
		if((n=write(STDOUT_FILENO,buf,n)) == -1)
			perror("write error"),exit(1);
		fprintf(stderr,"}\n");
	}

	fdin = dup(STDIN_FILENO);

	printf("-------after dup(STDIN_FILENO) to fdin=%d---------\n", fdin);
	list_dir("/dev/fd","\t");

	printf("-------after close(STDIN_FILENO)---------\n");
	close(STDIN_FILENO);
	list_dir("/dev/fd","\t");


	while((n=read(fdin, buf, sizeof(buf))) > 0)
	{
		fprintf(stderr,"[read() %d bytes]:{", n);
		if((n=write(STDOUT_FILENO,buf,n)) == -1)
			perror("write error"),exit(1);
		fprintf(stderr,"}\n");
	}

	if(n == -1)
		perror("read error"),exit(1);
	if(n == 0)
		printf("\n(file-end inidicated by read() == 0)\n");

	exit(0);
}
