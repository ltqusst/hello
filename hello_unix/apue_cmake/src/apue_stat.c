#include "apue.h"
#include <fcntl.h>           /* Definition of AT_* constants */
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char * argv[])
{
	int i,n;
	struct stat buf;
	char * ptr;
	int len,off;
	char info[4096];
	char perm[1+4*3+1]={0};
	char acc[3+3+1]={0};

	char str_mtim[128];

	printf("uid:gid=%d:%d \teuid:ugid=%d:%d \tpid=%ld\n",
			getuid(),getgid(),
			geteuid(),getegid(),
			(long)getpid());

	fflush(stdout);

	for(i=1-3;i<argc;i++)
	{
		//first 3 index (-2,-1,0) will be used to
		//referencing standard input/output/error
		char fname[1024];

		if(i < 1){
			sprintf(info, "/proc/self/fd/%d", i+2);
			n = readlink(info, fname, sizeof(fname));
			fname[n] = 0;

			if(fstat(i+2, &buf) < 0){
				printf("fd=%d ", i+2); fflush(stdout);
				perror("fstat error"); continue;
			}
		}else{
			strcpy(fname , argv[i]);
			if(lstat(argv[i], &buf) < 0){
				printf("%s ", argv[i]); fflush(stdout);
				perror("lstat error"); continue;
			}
		}

		info[0] = 0;

		if(S_ISREG(buf.st_mode)) ptr="file";
		else if(S_ISDIR(buf.st_mode)) { ptr="dir";
			int cnt=0;
			DIR * dir = opendir(argv[i]);
			struct dirent * dp;
			sprintf(info, "\n");
			while((dp=readdir(dir)) != NULL){
				if(++cnt <= 10)
					sprintf(info+strlen(info), "\t%d \t%s\n", dp->d_ino, dp->d_name);
			}
			sprintf(info+strlen(info),
					cnt>10?"\t...\t(%d) more entries...\n":"\t   \t(%d) entries total\n",
					cnt>10?(cnt-10):cnt);
			closedir(dir);
		}
		else if(S_ISCHR(buf.st_mode)) ptr="char";
		else if(S_ISBLK(buf.st_mode)) ptr="block";
		else if(S_ISFIFO(buf.st_mode)) ptr="pipe";
		else if(S_ISLNK(buf.st_mode)) {
				ptr="link(symbolic)";
				strcat(info,"readlink()="); off=strlen(info);
				if((len = readlink(argv[i], info + off, sizeof(info) - off))==-1)
					perror("readlink error");
				info[off + len] = 0;
		}
		else if(S_ISSOCK(buf.st_mode)) ptr="socket";
		else ptr="-";

		perm[0] = ptr[0];
		perm[1] = S_ISUID & buf.st_mode?'s':'-';
		perm[2] = S_ISGID & buf.st_mode?'g':'-';
		perm[3] = 01000 & buf.st_mode?'v':'-';

		perm[4] = S_IRUSR & buf.st_mode?'r':'-';
		perm[5] = S_IWUSR & buf.st_mode?'w':'-';
		perm[6] = S_IXUSR & buf.st_mode?'x':'-';

		perm[7] = S_IRGRP & buf.st_mode?'r':'-';
		perm[8] = S_IWGRP & buf.st_mode?'w':'-';
		perm[9] = S_IXGRP & buf.st_mode?'x':'-';

		perm[10] = S_IROTH & buf.st_mode?'r':'-';
		perm[11] = S_IWOTH & buf.st_mode?'w':'-';
		perm[12] = S_IXOTH & buf.st_mode?'x':'-';


		acc[0] = access(fname, R_OK) == 0?'R':'-';
		acc[1] = access(fname, W_OK) == 0?'W':'-';
		acc[2] = access(fname, X_OK) == 0?'X':'-';

		acc[3] = faccessat(AT_FDCWD, fname, R_OK, AT_EACCESS)==0?'R':'-';
		acc[4] = faccessat(AT_FDCWD, fname, W_OK, AT_EACCESS)==0?'W':'-';
		acc[5] = faccessat(AT_FDCWD, fname, X_OK, AT_EACCESS)==0?'X':'-';

		//format the date-time of the Unix time-stamp
		strftime(str_mtim, sizeof(str_mtim), "%b %d,%Y %H:%M:%S",
				localtime(&buf.st_mtim));

		struct passwd * perm_usr = getpwuid(buf.st_uid);
		struct group * perm_group = getgrgid(buf.st_gid);

		printf("%7d %s(0%06o) %d %10s %10s %8d(%d) access:%s\t%s\t%s   info: [%s]\n",
				buf.st_ino, perm, buf.st_mode, buf.st_nlink,
				perm_usr->pw_name, perm_group->gr_name,
				buf.st_size, buf.st_blocks,
				acc, str_mtim,
				fname, info);
	}
	exit(0);
}
