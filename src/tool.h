/*
 * tool.h
 *
 *  Created on: Dec 25, 2015
 *      Author: Eason
 */

#ifndef TOOL_H_
#define TOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*一些恒定的参数*/
#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /*  Backlog for listen()   */
#define RIO_BUFSIZE 8192

extern char **environ; /* defined by libc */

/*definde struct*/
typedef struct sockaddr SA;
typedef struct {
	int rio_fd; //缓冲区的描述符
	int rio_cnt; //未读字节的缓冲区
	char *rio_bufptr;
	char rio_buf[RIO_BUFSIZE]; //内部缓冲区
} rio_t;

void unix_error(char *msg);
void dns_error(char *msg);

int open_clientfd(char *hostname, int portno);
int Open_clientfd(char *hostname, int port);

int open_listenfd(int port);
int Open_listenfd(int port);

/*接收客户端发来的请求*/
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/*Unix I/O*/
void rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

void Rio_readinitb(rio_t *rp, int fd);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void Rio_writen(int fd, void *usrbuf, size_t n);

void Close(int fd);
int Open(const char *pathname, int flags, mode_t mode);

/*Unix memory*/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);

pid_t Fork(void);
int Dup2(int fd1, int fd2);
pid_t Wait(int *status);

#endif /* TOOL_H_ */
