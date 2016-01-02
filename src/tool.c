/*
 * tool.c
 *
 *  Created on: Dec 25, 2015
 *      Author: Eason
 */

#include "tool.h"

#define LISTENQ  1024  /* second argument to listen() */

/* unix-style error */
void unix_error(char *msg) {
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

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

	if ((hp = gethostname(hostname, sizeof(hostname))) == NULL) {
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

int Open_clientfd(char *hostname, int port) {
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

	if (bind(listenfd, (SA*) &serveraddr, sizeof(serveraddr)) < 0) {
		return -1;
	}

	if (listen(listenfd, LISTENQ) < 0) {
		return -1;
	}

	return listenfd;
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
 * rp->rio_cnt个未读的字节，如果缓冲区为空，会通过read函数再填满它。
 * 这个read电泳收到一个不足值并不是错误，只不过读缓冲区填充了一部分，
 * 一旦缓冲区非空，rio_read就充缓冲区拷贝n和rp->cnt中较小的自己到用户缓冲区，
 * 并返回拷贝字节数.
 */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
	int cnt;

	while (rp->rio_cnt <= 0) { /*如果为空，就填满*/
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if (rp->rio_cnt < 0) {
			if (errno != EINTR) /* interrupted by sig handler return */
				return -1;
		} else if (rp->rio_cnt == 0) /* EOF */
			return 0;
		else
			rp->rio_bufptr = rp->rio_buf; /* reset buffer ptr */
	}

	/*缓冲区为非空，将n和rp->cnt拷贝到用户缓冲区*/
	cnt = n;
	if (rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}

/**
 * 每次调用都从缓冲区读取一个字节，然后判断这个字节是否是结尾的换行符
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
	int n, rc;
	char c, *bufp = usrbuf;

	for (n = 1; n < maxlen; n++) {
		if ((rc = rio_read(rp, &c, 1)) == 1) {
			*bufp++ = c;
			if (c == '\n') {
				break;
			}
		} else if (rc == 0) {
			if (n == 1) {
				return 0; //EOF，没有读取任何数据
			} else {
				break; //EOF,读取了一些数据了
			}
		} else {
			return -1; //error
		}
	}

	*bufp = 0;
	return n;
}

/**
 * 关联文件描述符，并重置缓冲区
 */
void rio_readinitb(rio_t *rp, int fd) {
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

/**
 * 对rio_readlineb的封装，增添错误提示符
 */
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
	ssize_t rc;

	if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
		unix_error("Rio_readlineb error");
	return rc;
}

/**
 * 将userbuf写入到fd中
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n) {
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = usrbuf;

	while (nleft > 0) {
		if ((nwritten = write(fd, bufp, nleft)) <= 0) {
			if (errno == EINTR) {/* interrupted by sig handler return */
				nwritten = 0; /* and call write() again */
			} else {
				return -1; /* errno set by write() */
			}
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}

/**
 * 对rio_writen进行封装
 * 将userbuf写入到fd中
 */
void Rio_writen(int fd, void *usrbuf, size_t n) {
	if (rio_writen(fd, usrbuf, n) != n) {
		unix_error("Rio_writen error");
	}
}

/**
 * 对 close 函数的封装
 */
void Close(int fd) {
	int rc;

	if ((rc = close(fd)) < 0) {
		unix_error("Close error");
	}
}

/**
 * 对Unix open函数的封装
 */
int Open(const char *pathname, int flags, mode_t mode) {
	int rc;

	if ((rc = open(pathname, flags, mode)) < 0) {
		unix_error("Open error");
	}
	return rc;
}

/*对Unix 映射文件到内存的函数封装*/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
	void *ptr;

	if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1)) {
		unix_error("mmap error");
	}
	return (ptr);
}

void Munmap(void *start, size_t length) {
	if (munmap(start, length) < 0) {
		unix_error("munmap error");
	}
}

/**
 * 对fork 函数的封装
 */
pid_t Fork(void) {
	pid_t pid;

	if ((pid = fork()) < 0) {
		unix_error("Fork error");
	}
	return pid;
}

int Dup2(int fd1, int fd2) {
	int rc;

	if ((rc = dup2(fd1, fd2)) < 0) {
		unix_error("Dup2 error");
	}
	return rc;
}

void Execve(const char *filename, char * const argv[], char * const envp[]) {
	if (execve(filename, argv, envp) < 0) {
		unix_error("Execve error");
	}
}

/**
 * 对 wait函数的封装
 */
pid_t Wait(int *status) {
	pid_t pid;

	if ((pid = wait(status)) < 0)
		unix_error("Wait error");
	return pid;
}

