#ifndef __TCP_SRV_H__
#define __TCP_SRV_H__

#include "common.h"

#define SERVER_THREAD_MAX       5

typedef struct __THREAD_PARAM{
    int sock_fd;
    int client_id;
}ThreadParam, *PThreadParam;

#endif
