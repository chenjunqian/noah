/*
 * tool.h
 *
 *  Created on: Dec 25, 2015
 *      Author: Eason
 */

#ifndef TOOL_H_
#define TOOL_H_

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/mman.h>
#include <fcntl.h>

/*一些恒定的参数*/
#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ        (1024)   /*  Backlog for listen()   */
#define RIO_BUFSIZE 8192

extern char **environ; /* defined by libc */

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);

int open_clientfd(char *hostname, int portno);
int open_listenfd(int portno);

int Open_clientfd(char *hostname, int port);
int Open_listenfd(int port);

/*接收客户端发来的请求*/
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/*Unix I/O*/
void rio_readinitb(rio_t *rp, int fd);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
int Open(const char *pathname, int flags, mode_t mode);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void Rio_writen(int fd, void *usrbuf, size_t n);
void Close(int fd);

/*Unix memory*/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);

pid_t Fork(void);
int Dup2(int fd1, int fd2);
pid_t Wait(int *status);

typedef struct{
	int rio_fd;//缓冲区的描述符
	int rio_cnt;//未读字节的缓冲区
	int *rio_bufptr;
	int rio_buf[RIO_BUFSIZE];//内部缓冲区
} rio_t;

#endif /* TOOL_H_ */
