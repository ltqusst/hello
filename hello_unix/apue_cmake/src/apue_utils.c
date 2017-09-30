/*
 * apue_utils.c
 *
 *  Created on: Sep 13, 2017
 *      Author: tingqian
 */

#include "apue.h"
#include "apue_utils.h"
#include <syslog.h>
#include <sys/resource.h>
#include <fcntl.h>

void daemonize(const char * cmd, int keep_stdio)
{
	int 			i,fd0,fd1,fd2;
	pid_t			pid;
	struct rlimit	rl;
	struct sigaction sa;

	/*
	 * Create file creation mask
	 */
	umask(0);

	/* get maximum number of file descriptors */
	if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
		fprintf(stderr, "%s: can't get file limit", cmd);

	/* become a session leader so that we :
	 *  1.become a new process-group leader
	 *  2.lose any controlling TTY that we may inherited
	 */
	/* first need to live as orphan:
	 * just to make sure we are not a process-group leader*/
	if((pid = fork()) < 0){
		perror("daemonize(): fork error");
		exit(1);
	} else if (pid != 0) /* parent */
		exit(0);

	printf("daemonize(): fork() as orphan (pid=%ld)\n", (long)getpid());

	setsid();

	printf("daemonize(): setsid() done\n");

	/* make sure TTY close won't close us */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		printf("daemonize(): can't ignore SIGHUP\n"), exit(0);

	printf("daemonize(): SIGHUP ignored\n");

	/* make sure we will not be allocated any controlling TTY
	 * even when we open a TTY devices(by specify O_NOCTTY) */
	if((pid = fork()) < 0){
		perror("daemonize(): fork error");
		exit(1);
	} else if (pid != 0) /* parent */
		exit(0);

	printf("daemonize(): fork again: (pid=%ld)\n", (long)getpid());

	/* Change the current working directory to the root so
	 * we won't prevent file system from being unmounted */

	if(chdir("/") < 0)
		perror("daemonize(): chdir to root error"), exit(1);

	printf("daemonize(): chdir to /\n");

	/* Close all file descriptor */
	if(rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for(i=keep_stdio?3:0; i<rl.rlim_max; i++)
		close(i);

	if(!keep_stdio){
		/* Attatch file descriptor 0,1,2 to /dev/null */
		fd0 = open("/dev/null", O_RDWR);
		fd1 = dup(0);
		fd2 = dup(0);
	}

	printf("daemonize(): you can't see this line unless keep_stdio=%d\n", keep_stdio);

	/* Initialize the log file */
	openlog(cmd, LOG_CONS|LOG_PID, LOG_DAEMON);
	if((fd0 != 0 || fd1 != 1 || fd2 != 2) && (keep_stdio == 0)){
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
				fd0,fd1,fd2);
		exit(1);
	}
}

//=====================================================================
//
void set_fd_closexec(int fd, int bset)
{
	int val;
	if((val = fcntl(fd, F_GETFD, 0)) < 0)
		perror("fnctl(, F_GETFD) error"), exit(1);

	if(bset)
		val |= FD_CLOEXEC;
	else
		val &= ~FD_CLOEXEC;

	if(fcntl(fd, F_SETFD, val) < 0)
		perror("fnctl(, F_SETFD) error"), exit(1);

}

void set_file_flags(int fd, int flags, int bset)
{
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0)
		perror("fnctl(, F_GETFL) error"), exit(1);

	if(bset)
		val |= flags;
	else
		val &= ~flags;

	if(fcntl(fd, F_SETFL, val) < 0)
		perror("fnctl(, F_SETFL) error"), exit(1);
}

//=====================================================================
// SOCKET

#define CASE(name) case name: return #name; break;
#define DEF(name,val) default: sprintf(strret, #name"[%d]", val); break;


const char * strAF(int val)
{
	static char strret[1024];
	switch(val)
	{
	CASE(AF_INET);
	CASE(AF_INET6);
	CASE(AF_UNIX);
	CASE(AF_UNSPEC);
	DEF(AF_UNKNOWN,val);
	}
	return strret;
}

const char * strTYPE(int val)
{
	static char strret[1024];
	switch(val)
	{
	CASE(SOCK_STREAM);
	CASE(SOCK_DGRAM);
	CASE(SOCK_SEQPACKET);
	CASE(SOCK_RAW);
	DEF(SOCK_UNKNOWN,val);
	}
	return strret;
}
const char * strPROTO(int val)
{
	struct protoent * p = getprotobynumber(val);
	if(p)
		return p->p_name;
	return "Unknow";
}
char * strSockaddr(struct sockaddr *sa)
{
	static char ret[1024];
	char abuf[INET_ADDRSTRLEN];

	//inet socket?
	switch(sa->sa_family)
	{
	case AF_INET:{
		struct sockaddr_in * sinp = (struct sockaddr_in *)sa;
		const char * addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, sizeof(abuf));
		if(sinp->sin_addr.s_addr == INADDR_ANY)
			strcat(abuf,"(INADDR_ANY)");
		else if (sinp->sin_addr.s_addr == INADDR_BROADCAST)
			strcat(abuf,"(INADDR_BROADCAST)");
		sprintf(ret, "%10s:%s:%d",
					strAF(sa->sa_family),
					addr?abuf:"unknown",
					ntohs(sinp->sin_port));
	}
	break;
	case AF_UNIX:{
		struct sockaddr_un *sunp = (struct sockaddr_un *)sa;
		sprintf(ret, "%10s:%s",strAF(sa->sa_family),sunp->sun_path);
	}
	break;
	default:
		sprintf(ret, "%10s:...", strAF(sa->sa_family));
		break;
	}
	return ret;
}

const char * strBIN(char * data, int len, const char * line_header, const char * name)
{
	static char ret[1024];
	int i,j;
	sprintf(ret,"%s ==== varible:%s @0x%p ====\n", line_header, name, data);
	for(i=0; i<len; i+=16)
	{
		sprintf(ret+strlen(ret), "%s 0X%08X:", line_header, i);

		for(j=0;j<16 && (i+j<len);j++)
			sprintf(ret+strlen(ret), "%02X ", data[j + i]);

		sprintf(ret+strlen(ret), "\n");
	}
	return ret;
}

void printf_sockaddr(struct sockaddr *sa)
{
	printf("%s", strSockaddr(sa));
}
void printf_addrinfo(struct addrinfo * aip)
{
	printf("name:%20s %10s (%12s,%-8s) ",
						aip->ai_canonname?aip->ai_canonname:"???",
						strAF(aip->ai_family),
						strTYPE(aip->ai_socktype),
						strPROTO(aip->ai_protocol));
	printf_sockaddr(aip->ai_addr);
	printf("\n");
}


int sock_connect(int domain,
				 int type,
				 int protocol,
				 const struct sockaddr *addr, socklen_t alen)
{
	int numsec, fd;
#define MAXSLEEP 32
	/*
	 * Try to connect with exponential backoff.
	 */
	for(numsec=1;numsec<MAXSLEEP;numsec *= 2){
		if((fd = socket(domain, type, protocol)) < 0)
			return -1;
		if(connect(fd, addr, alen) == 0)
			return fd;

		close(fd);
		/* Delay before try again */
		if(numsec *2 < MAXSLEEP)
			sleep(numsec);
	}
	return -1;
}
int sock_initserver(int type,
		const struct sockaddr *addr, socklen_t alen,
		int qlen)
{
	int fd;
	int err=0;
	int reuse = 1;
	//Protocol is determined by defaults according to type
	if((fd=socket(addr->sa_family, type, 0)) < 0)
		return -1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		goto errout;
	if(bind(fd, addr, alen) < 0)
		goto errout;
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		if(listen(fd,qlen) < 0)
			goto errout;
	}
	return (fd);
errout:
	err = errno;
	close(fd);
	errno = err;//to skip errno from close(fd);
	return (-1);
}

