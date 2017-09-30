#include "apue.h"
#include <fcntl.h>
#include <pwd.h>
void printf_uidgid(FILE *fp,const char *name, int uid, int gid)
{
	fprintf(fp, "%s=%d:%d",name, uid, gid);
	fprintf(fp, "(%s:", getpwuid(uid)->pw_name);
	fprintf(fp, "%s)\n", "??");
}

int main(int argc, char * argv[])
{
	char buf[MAXLINE];
	int n, fd;

	if(argc < 2)
		fprintf(stderr,"one file-name argument needed!"), exit(-1);

	printf_uidgid(stderr," uid: gid",(int)getuid(),(int)getgid());
	printf_uidgid(stderr,"euid:egid",(int)geteuid(),(int)getegid());
	
	if((fd = open(argv[1],O_RDONLY)) == -1)
		perror("open failed"), exit(-1);

	while((n=read(fd, buf, MAXLINE)) > 0){
		if(write(STDOUT_FILENO, buf, n) < 0)
			perror("write error"), exit(-1);
	}
	if(n < 0)
		perror("read error"), exit(-1);

	exit(0);
}

