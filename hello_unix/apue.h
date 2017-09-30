#ifndef _APUE_H
#define APUE_H

#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/termios.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include <errno.h>

#define MAXLINE 4096

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define DIR_MODE  (FILE_MODE|S_IXUSR|S_IXGRP|S_IXOTH)

typedef void Sigfunc(int);

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

void err_puts(int error_number, const char *fmt,...);


#endif
