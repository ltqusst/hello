/*
 * apue_sockserver1.c
 *
 *  Created on: Sep 16, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include "apue_utils.h"

#define QLEN 128

static const char * servname = "server1";

void serve_stream(int sockfd)
{
	int client_fd;
	FILE *fp;
	char buf[2048];
	struct sockaddr sa;
	socklen_t salen;
	struct msghdr msg;
	int n;

	set_fd_closexec(sockfd, 1);
	for(;;){
		salen = sizeof(sa); //don't forget this, or accept will not return sa correctly
		if((client_fd = accept(sockfd, &sa, &salen)) < 0){
			syslog(LOG_ERR, "%s: accept error, exit.", servname);
			exit(1);
		}

		set_fd_closexec(client_fd, 1);

		printf("%s: sockfd=%d, client_fd=%d\n", servname, sockfd, client_fd);

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		dup2(client_fd, STDIN_FILENO);
		dup2(client_fd, STDOUT_FILENO);
		dup2(client_fd, STDERR_FILENO);

		execl("/bin/bash", (char *)0);

/*
		while(1){
			n = recv(client_fd, buf, sizeof(buf)-1, 0);
			if(n <= 0){
				printf("%s: recv error:%s\n", servname, strerror(errno));
				break;
			}
			buf[n] = 0;

			if((fp = popen(buf,"r")) == NULL){
				sprintf(buf, "%s: popen error(%s), exit.", servname, strerror(errno));
				syslog(LOG_ERR, "%s", buf);
				exit(1);
			}else{
				while(fgets(buf, sizeof(buf), fp) != NULL){
					send(client_fd, buf, strlen(buf), 0);
				}
				//msg.msg_iovlen = 3;
				//msg.msg_iov = (struct iovec *)malloc(msg.msg_iovlen*sizeof(struct iovec));
				//msg.msg_controllen = CMSG_LEN(sizeof(int));
				//msg.msg_control = malloc(msg.msg_controllen);
				pclose(fp);
			}
		}
*/
		printf("%s: client connected & served: %s \n", servname, strSockaddr(&sa));
		close(client_fd);
	}
}





int main(int argc, char * argv[])
{
	int sockfd, err;
	struct addrinfo hint, *aip, *ailist;
	struct sockaddr_in sain = {0};
	int oc;

	servname = argv[0];
	while((oc=getopt(argc, argv,"p:")) != -1)
	{
		switch(oc)
		{
		case 'p':
			servname = optarg;
			break;
		}
	}
	printf("servname=%s\n", servname);

	// here we can keep standard IO open to make debug easier.
	daemonize(argv[0], 1);

	// there is a service(refer to 'man service') database in Unix system
	// we can specify any port as we wish without pay any respect to it
	// just by passing port number as a string to the second argument of getaddrinfo().
	//
	// OR we needs to edit /etc/services add a record of our service/port
	// configuration. and use service-DB API like getaddrinfo() to abstain
	// the port.

	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_socktype = SOCK_STREAM;
	if((err = getaddrinfo(NULL,servname, &hint, &ailist)) != 0){
		syslog(LOG_ERR, "%s: getaddrinfo error: %s",
				servname, gai_strerror(err));
		exit(1);
	}

	for(aip = ailist; aip; aip = aip->ai_next){

		printf("%s: try to init server at %s ...\n", servname, strSockaddr(aip->ai_addr));

		if((sockfd = sock_initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0){
			struct sockaddr sa;
			socklen_t slen = sizeof(sa); //don't forget initialize this field
			if(getsockname(sockfd, &sa, &slen) == 0)
				printf("%s: server sockaddr is %s\n", servname, strSockaddr(&sa));
			else
				printf("%s: server initOK but getsockname error\n", servname);

			syslog(LOG_INFO, "%s: server start ", servname);
			serve_stream(sockfd);
			syslog(LOG_INFO, "%s: server end", servname);
			exit(0);
		}
	}

	fprintf(stderr,"%s: can't initserver. exit\n", servname);

	exit(1);
}
