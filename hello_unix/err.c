#include "apue.h"
#include <errno.h>
#include <stdarg.h>

/* 
  error_number is zero do not mean no error
  because APUE said no sys function will set errno to zero

  	1.only check errno when sysfunc's return shows something wrong 
	2.do not check if errno==0 for no error 
 */
void err_puts(int error_number, const char *fmt,...)
{
	char buf[MAXLINE];
	va_list ap;
	va_start(ap,fmt);	
	vsnprintf(buf,MAXLINE-1,fmt,ap);
	va_end(ap);

	if(error_number != 0)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1,": %s",
				strerror(error_number));
	strcat(buf,"\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}


