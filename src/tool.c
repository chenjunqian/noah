/*
 * tool.c
 *
 *  Created on: Dec 25, 2015
 *      Author: Eason
 */

#include "tool.h"

#define LISTENQ  1024  /* second argument to listen() */

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

/*  Read a line from a socket  */

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char c, *buffer;

	buffer = vptr;

	for (n = 1; n < maxlen; n++) {

		if ((rc = read(sockd, &c, 1)) == 1) {
			*buffer++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return 0;
			else
				break;
		} else {
			if ( errno == EINTR)
				continue;
			return -1;
		}
	}

	*buffer = 0;
	return n;
}

/*  Write a line to a socket  */

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *buffer;

	buffer = vptr;
	nleft = n;

	while (nleft > 0) {
		if ((nwritten = write(sockd, buffer, nleft)) <= 0) {
			if ( errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		buffer += nwritten;
	}

	return n;
}

/**
 * 由hostname和port创建连接
 */
int open_clientfd(char *hostname, int port) {
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;

	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if ((hp = gethostname(hostname)) == NULL) {
		return -2;
	}

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;

	bcopy((char *) hp->h_addr_list[0], (char *) serveraddr.sin_addr.s_addr,
			hp->h_length);
	serveraddr.sin_port = htonl(port);

	if (connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			< 0) {
		return -1;
	}

	return clientfd;
}

/**
 * 打开并返回一个sock文件描述符
 */
int open_listenfd(int port) {
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	//创建一个socket文件描述符
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	//检测端口是否被占用
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) optval,
			sizeof(int)) < 0) {
		return -1;
	}

	//listenfd将与端口绑定起来
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short )port);

	if (bind(listenfd, (struct sockaddr *) serveraddr, sizeof(serveraddr))
			< 0) {
		return -1;
	}

	if (listen(listenfd, LISTENQ) < 0) {
		return -1;
	}

	return listenfd;
}

/**
 *
 */
int Open_listenfd(char *hostname, int port) {
	int rc;
	if ((rc = open_clientfd(hostname, port)) < 0) {
		if (rc == -1) {
			unix_error("Open_clientfd Unix error");
		} else {
			dns_error("Open_clientfd DNS error");
		}
	}

	return rc;
}

int Open_listenfd(int port) {
	int rc;

	if ((rc = open_listenfd(port)) < 0)
		unix_error("Open_listenfd error");
	return rc;
}

/**
 * 接收客户端发来的请求
 */
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
	int rc;

	if ((rc = accept(s, addr, addrlen)) < 0)
		unix_error("Accept error");
	return rc;
}

/**
 * 对Unix系统中 read()函数的封装，是一个带有缓冲区的read()版本，
 * 当调用rio_read要求读n个字节时，读缓冲区内有
 * rp->rio_cnt个未读的自己，如果缓冲区为空，会通过read函数再填满它。
 * 这个read电泳收到一个不足值并不是错误，只不过读缓冲区填充了一部分，
 * 一旦缓冲区非空，rio_read就充缓冲区拷贝n和rp->cnt中较小的自己到用户缓冲区，
 * 并返回拷贝字节数.
 */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
	int cnt;

	while (rp->rio_cnt <= 0) { /* refill if buf is empty */
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if (rp->rio_cnt < 0) {
			if (errno != EINTR) /* interrupted by sig handler return */
				return -1;
		} else if (rp->rio_cnt == 0) /* EOF */
			return 0;
		else
			rp->rio_bufptr = rp->rio_buf; /* reset buffer ptr */
	}

	/* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
	cnt = n;
	if (rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
	int n, rc;
	char c, *bufp = usrbuf;

	for (n = 1; n < maxlen; n++) {
		if ((rc = rio_read(rp, &c, 1)) == 1) {

		}
	}
}

