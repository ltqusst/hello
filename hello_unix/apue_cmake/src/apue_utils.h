/*
 * apue_utils.h
 *
 *  Created on: Sep 13, 2017
 *      Author: tingqian
 */

#ifndef APUE_UTILS_H_
#define APUE_UTILS_H_

void daemonize(const char * cmd, int keep_stdio);

//function's arguments needs these headers
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

const char * strAF(int val);
const char * strTYPE(int val);
const char * strPROTO(int val);
const char * strBIN(char * data, int len, const char * line_header, const char * name);
#define strVAR(name,lineH) strBIN((char*)&name,sizeof(name),lineH,#name)
char * strSockaddr(struct sockaddr *sa);

void printf_sockaddr(struct sockaddr *sa);
void printf_addrinfo(struct addrinfo * aip);


void set_fd_closexec(int fd, int bset);
void set_file_flags(int fd, int flags, int bset);
#define fl_set(fd, flags) set_file_flags(fd, flags, 1)
#define fl_clr(fd, flags) set_file_flags(fd, flags, 0)


int sock_connect(int domain,
				 int type,
				 int protocol,
				 const struct sockaddr *addr, socklen_t alen);
int sock_initserver(int type,
					const struct sockaddr *addr, socklen_t alen,
					int qlen);


#endif /* APUE_UTILS_H_ */
