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

int main(int argc, char **argv[]) {
	int list_s; /*  listening socket          */
	int conn_s; /*  connection socket         */
	short int port; /*  port number               */
	struct sockaddr_in servaddr; /*  socket address structure  */
	char buffer[MAX_LINE]; /*  character buffer          */
	char *endptr; /*  for strtol()              */

	/*获取端口号，如果没有获取到就用默认的*/
	if (argc == 2) {
		port = strtol(argv[1], &endptr, 0);
		if (*endptr) {
			fprintf(stderr, "NoahServer: Invalid port number.\n");
			exit(EXIT_FAILURE);
		}
	} else if (argc < 2) {
		port = ECHO_PORT;
	} else {
		fprintf(stderr, "NoahServer: Invalid arguments.\n");
		exit(EXIT_FAILURE);
	}

	/*  Create the listening socket  */
	if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "NoahServer: Error creating listening socket.\n");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));/*清空servaddr*/
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr = htonl(INADDR_ANY);/*把本机字节顺序转化为网络字节顺序*/
	servaddr.sin_port = htons(port);/*将主机字节顺序转换为网络字节顺序*/

	/**
	 * 绑定端口
	 */
	if (bind(list_s, (struct sockaddr_in *) servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "NoahServer: Error calling bind()\n");
		exit(EXIT_FAILURE);
	}

	if (listen(list_s, LISTENQ) < 0) {
		fprintf(stderr, "ECHOSERV: Error calling listen()\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * response request to client
	 */
	while (1) {

		if ((conn_s = accept(list_s,NULL,NULL))<0) {
		    fprintf(stderr, "NoahServer: Error calling accept()\n");
		    exit(EXIT_FAILURE);
		}

		Readline(conn_s, buffer, MAX_LINE-1);
		Writeline(conn_s, buffer, strlen(buffer));

		/**
		 * 关闭socket
		 */
		if ( close(conn_s) < 0 ) {
		    fprintf(stderr, "ECHOSERV: Error calling close()\n");
		    exit(EXIT_FAILURE);
		}
	}
}

