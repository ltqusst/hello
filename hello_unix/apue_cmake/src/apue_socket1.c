/*
 * apue_socket1.c
 *
 *  Created on: Sep 13, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include "apue_utils.h"

void enumproto();
void enumserv();
void enumnet();

int main(int argc, char * argv[])
{
	int i;
	char hname[1024];
	if(gethostname(hname, sizeof(hname)) == -1)
		perror("gethostname error"),exit(1);

	printf("HostName:%s\n", hname);

	if(argc == 1)
	{
		enumnet();
		enumproto();
		enumserv();
	}

	if(argc >= 1)
	{
		int err;
		struct addrinfo hint, *ailist, *aip;
		memset(&hint, 0, sizeof(hint));
		hint.ai_flags = AI_PASSIVE;
		if((err = getaddrinfo(NULL,"http", &hint, &ailist)) != 0){
			fprintf(stderr,"getaddrinfo error:%s\n", gai_strerror(err));
			exit(1);
		}
		for(aip = ailist; aip != NULL; aip = aip->ai_next)
			printf("%10s:","passive"),printf_addrinfo(aip);

		hint.ai_flags = AI_CANONNAME;
		if((err = getaddrinfo(argv[1],
							  argc>=3 ? argv[2]:NULL, &hint, &ailist)) != 0){
			fprintf(stderr,"getaddrinfo error:%s\n", gai_strerror(err));
			exit(1);
		}


		if(ailist == NULL)
			printf("getaddrinfo() return NULL\n");

		for(aip = ailist; aip != NULL; aip = aip->ai_next)
			printf("%10s:",argv[1]), printf_addrinfo(aip);
	}

	exit(0);
}


void enumnet()
{
	struct netent * pn;

	setnetent(1);
	while((pn = getnetent()) != NULL)
	{
		int k=0;
		printf("name:%s num:%d addrtype:%d ",
				pn->n_name, pn->n_net, pn->n_addrtype);

		while(pn->n_aliases && pn->n_aliases[k])
			printf("%s ", pn->n_aliases[k++]);
		printf("\n");
	}
	endnetent();
}
void enumproto()
{
	struct protoent * pp;
	printf("===============[PROTO entries]======================\n");
	setprotoent(1);
	while((pp=getprotoent()) != NULL)
	{
		int k = 0;
		printf("[%d]:%s ... ", pp->p_proto, pp->p_name);
		while(pp->p_aliases && pp->p_aliases[k])
		{
			printf("%s ", pp->p_aliases[k]);
			k++;
		}
		printf("\n");
	}
	endprotoent();
}
void enumserv()
{
	struct servent *ps;
	printf("===============[SERVICE entries]======================\n");
	setservent(1);
	while((ps=getservent()) != NULL)
	{
		int k = 0;
		printf("%s:%d %s ... ",
				ps->s_proto,ps->s_port,
				ps->s_name);

		while(ps->s_aliases && ps->s_aliases[k])
		{
			printf("%s ", ps->s_aliases[k]);
			k++;
		}
		printf("\n");
	}
	endservent();

}




