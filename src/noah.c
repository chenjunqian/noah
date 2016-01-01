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
void clienterror(int fd, char *cause, char errnum, char *shortmsg,
		char *longmsg);
void read_requesthead(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);

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
		clienterror(fd, method, "501", "Not implement",
				"Noah server has no such method");
		return;
	}

	read_requesthead(&rio);

}

/**
 * 打印错误信息
 */
void clienterror(int fd, char *cause, char errnum, char *shortmsg,
		char *longmsg) {
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
	sprintf(buf, "Content-length: %d\r\n\r\n", (int )strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}

/**
 * 读取请求报头中得信息
 */
void read_requesthead(rio_t *rp) {
	char buf[MAXLINE];

	Rio_readlineb(rp, buf, MAXLINE);
	while (strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%a", buf);
	}
	return;
}

/**
 * 解析URI为一个文件名，和一个可选的cgi参数字符串
 */
int parse_uri(char *uri, char *filename, char *cgiargs) {
	char *ptr;

	if (!strstr(uri, "cgi-bin")) { //如果请求的是静态内容
		strcpy(cgiargs, ""); //清楚CGI参数
		strcpy(filename, "."); //将路径URI转换为一个Unix路径
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == '/') { //如果URI已"/"结尾，将默认的文件名添加在后面
			strcat(filename, "home.html");
		}
		return 1;
	} else { //如果请求的是动态内容
		ptr = index(uri, '?'); //提取出CGI参数
		if (ptr) {
			strcpy(cgiargs, ptr + 1);
			*ptr = '\0';
		} else {
			strcpy(cgiargs, "");
		}

		//转换为一个Unix文件名
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}

/**
 * 提供静态文件
 */
void serve_static(int fd, char *filename, int filesize) {
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];
	get_filetype(filename, filetype);
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: Noah Web Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf));

	srcfd = Open(filename, O_RDONLY, 0); //打开文件filename，并获得文件描述符
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); //将文件映射到虚拟内存上
	Close(srcfd);
	Rio_writen(fd, srcp, filesize);
	Munmap(srcp, filesize);
}

/**
 * 判断文件类型
 */
void get_filetype(char *filename, char *filetype) {
	if (strstr(filename, ".html")) {
		strcpy(filetype, "text/html");
	} else if (strstr(filename, ".gif")) {
		strcpy(filetype, "image/gif");
	} else if (strstr(filename, ".jpg")) {
		strcpy(filetype, "image/jpeg");
	} else {
		strcpy(filetype, "text/plain");
	}
}

/**
 * 提供各种类型的动态内容
 */
void serve_dynamic(int fd, char *filename, char *cgiargs) {
	char buf[MAXLINE], *emptylist[] = { NULL };

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (Fork() == 0) {//派生一个子进程
		/*请求URI的CGI参数，初始化QUERY_STRING环境变量*/
		setenv("QUERY_STRING", cgiargs, 1);
		Dup2(fd, STDOUT_FILENO); /*重定向标准输出到已连接的描述符上*/
		Execve(filename, emptylist, environ); /*运行CGI程序*/
	}
	Wait(NULL); /*父进程对子进程等待回收*/
}
