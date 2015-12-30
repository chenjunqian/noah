/*
 ============================================================================
 Name        : noah.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Noah server in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "tool.h"

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)

void dealRequest(int fd);
void clienterror(int fd, char *cause, char errnum, char *shortmsg, char *longmsg);
void read_requesthead(rio_t *rp);

int main(int argc, char *argv[]) {

	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientadd;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[1]); //把字符串转换成整数

	listenfd = Open_listenfd(port);

	while (1) {
		clientlen = sizeof(clientadd);
		connfd = Accept(listenfd, (struct sockaddr *) clientadd, clientlen);

	}
}

/**
 * 处理HTTP事务
 * 读取和解析请求，目前只能支持GET请求，其他请求一律返回错误信息
 */
void dealRequest(int fd) {
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, fd);
	Rio_readlineb(&rio, buf, MAXLINE);
	scanf(buf, "%s %s %s", method, uri, version);
	if (strcasecmp(method, "GET")) {
		clienterror(fd,method,"501","Not implement","Noah server has no such method");
		return;
	}

	read_requesthead(&rio);


}

/**
 * 打印错误信息
 */
void clienterror(int fd, char *cause, char errnum, char *shortmsg, char *longmsg) {
	char buf[MAXLINE], body[MAXBUF];

	/**
	 * HTTP response body
	 */
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=" "ffffff" ">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Noah Web server</em>\r\n", body);

	/**
	 * 打印错误信息
	 */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/**
 * 读取请求报头中得信息
 */
void read_requesthead(rio_t *rp){
	char buf[MAXLINE];

	Rio_readlineb(rp,buf,MAXLINE);
	while(strcmp(buf,"\r\n")){
		Rio_readlineb(rp,buf,MAXLINE);
		printf("%a",buf);
	}

}
