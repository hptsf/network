#include "common.h"

int run_flag = 0;

void sig_handler(int sig)
{
    fprintf(stdout, "Get a signal[%d]\n", sig);
    run_flag = 0;
}

/* sock: set object
 * to  : timeout value, second
 */
int setSockTimeout(int sock, int to)
{
    struct timeval timeout = {to, 0};
    int ret = -1;

    ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    if(0 != ret){
        perror("setsockopt snd timeout failed");
        return -1;
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    if(0 != ret){
        perror("setsockopt rcv timeout failed");
        return -1;
    }

    return 0;
}
