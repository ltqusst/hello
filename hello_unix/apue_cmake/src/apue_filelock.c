/*
 * apue_filelock.c
 *
 *  Created on: Sep 9, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <fcntl.h>


int _file_lock(int cmd, int type,int fd,
			 int offset, int whence, int len, struct flock * p)
{
	struct flock lock;
	if(p == NULL) p = &lock;

	p->l_type = type;
	p->l_start = offset;
	p->l_whence = whence;
	p->l_len = len;

	return (fcntl(fd, cmd, p));
}

#define flock_read(fd, offset, when, len, p) \
	_file_lock(F_SETLK, F_RDLCK, fd, offset, when, len, p)
#define flock_readw(fd, offset, when, len, p) \
	_file_lock(F_SETLKW, F_RDLCK, fd, offset, when, len, p)

#define flock_write(fd, offset, when, len, p) \
	_file_lock(F_SETLK, F_WRLCK, fd, offset, when, len, p)
#define flock_writew(fd, offset, when, len, p) \
	_file_lock(F_SETLKW, F_WRLCK, fd, offset, when, len, p)

#define flock_unlock(fd, offset, when, len, p) \
	_file_lock(F_SETLK, F_UNLCK, fd, offset, when, len, p)



int main(int argc, char * argv[])
{
	int off = 0;
	char * pstr;
	int fd;

	off = atoi(argv[2]);
	pstr = argv[3];

	if((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) < 0)
		perror("open error"), exit(1);

	if(flock_write(fd, 5, SEEK_SET, 5, NULL) < 0)
		perror("flock_write error"),exit(1);

	if(lseek(fd, off, SEEK_SET) == -1)
		perror("lseek error"), exit(2);

	if(write(fd, pstr, strlen(pstr)) == -1)
		perror("write error"), exit(3);

	while(1)
	{
		pause();
	}

	close(fd);

	exit(0);
}



