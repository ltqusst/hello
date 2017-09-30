/*
 * apue_client1.c
 *
 *  Created on: Sep 17, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include "apue_utils.h"
#include <errno.h>

#include <poll.h>

void do_client(int sockfd)
{
	int n, m;
	char buf[2048];
	struct pollfd fds[2];

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN;
	fds[1].revents = 0;
	/* we need to read from STDIN or sockfd at the same time
	 * the best way is to use select */
	while(1){
		if(poll(fds, 2, -1) == -1)
			perror("poll error"), exit(1);

		if(fds[0].revents & POLLHUP)
			printf("STDIN is hang up\n"), exit(0);
		if(fds[1].revents & POLLHUP)
			printf("remote is hang up\n"), exit(0);

		if(fds[0].revents & POLLIN){
			if((n = read(STDIN_FILENO, buf, sizeof(buf))) == -1)
				perror("read error"), exit(1);
			if(n == 0){
				printf("read EOF\n"); break;
			}

			if((m=send(sockfd, buf, n, 0)) != n){
				printf("send bytes %d is not %d\n", m, n);
			}
		}

		if(fds[1].revents & POLLIN){
			if((n=recv(sockfd, buf, sizeof(buf), 0)) < 0)
				perror("recv error"),exit(1);
			if(n == 0){
				printf(">>> remote end\n"); break;
			}
			buf[n] = 0;
			printf(">>> remote: %s\n", buf);
		}
	}
	close(sockfd);
}

int main(int argc, char * argv[])
{
	int err;
	int oc;
	int sockfd;
	int cfg_port = 0;
	char * endptr;
	struct addrinfo hint, *aip, *ailist;
	struct sockaddr_in sain;
	char * hostname = "localhost";
	char * servname = "server1";

	while((oc=getopt(argc, argv,"p:")) != -1)
	{
		switch(oc)
		{
		case 'p':
			servname = optarg;
			break;
		}
	}

	if(optind >= argc)
		fprintf(stderr,"[INIT]: no host name/addr provided. default (%s) is used.\n", hostname);

	printf("[INIT]: remote target %s:%s\n",hostname, servname);

	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_STREAM;
	if((err = getaddrinfo(hostname, servname, &hint, &ailist)) != 0)
		fprintf(stderr,"getaddrinfo error: %s\n", gai_strerror(err)),exit(1);

	for(aip = ailist; aip!=NULL; aip = aip->ai_next){
		printf("[INIT]: connecting to %s...\n", strSockaddr(aip->ai_addr));
		if((sockfd = sock_connect(AF_INET, SOCK_STREAM, 0,
								  aip->ai_addr, aip->ai_addrlen)) < 0){
			err = errno;
		}else{
			do_client(sockfd);
			exit(0);
		}
	}

	fprintf(stderr,"can't connect to the host/service \n");
	exit(1);
	//
}

