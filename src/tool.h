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
#define LISTENQ        (1024)   /*  Backlog for listen()   */

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);

#endif /* TOOL_H_ */
