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

void requestHandler(int fd);
void readRequestHeader();
int parseUrl(char *uri, char *filename, char *cgiargs);
void serverStatic(int fd, char *filename, int filesize);
void getFiletype(char *filename, char *filetype);
void serverDynamic(int fd, char *filename, char *cgiargs);
void clientError(int fd, char *cause, char *errnum,
		 char *shortmsg, char *longmsg);

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}

/**
 * 接收Http的request和返回response
 */
void requestHandler(int ){

}

/**
 * 接收Http的Head和解析Head
 */
void readRequestHeader(){

}

/**
 * 解析url中得filename和CGI args
 */
int parseUrl(char *url,char *filename,char *cgiargs){

	return 0;
}


void serverStatic(int fd, char *filename, int filesize){

}

/**
 * 获取文件的类型
 */
void getFiletype(char *filename, char *filetype){

}

void serverDynamic(int fd, char *filename, char *cgiargs){

}

/**
 * 返回错误信息
 */
void clientError(int fd, char *cause, char *errnum,
		 char *shortmsg, char *longmsg){

}
