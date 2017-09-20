#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <signal.h>
#include <pthread.h>

#define BUF_MAX_LEN             512
#define DEFAULT_PORT            56789
#define DEFAULT_QUEUE_LENGTH    3

extern int run_flag;

void sig_handler(int sig);
int setSockTimeout(int sock, int to);

#endif
