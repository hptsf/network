#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

int run_flag = 0;

void sig_handle(int sig)
{
    fprintf(stdout, "Get a signal[%d]\n", sig);
    run_flag = 0;
}

void echo_cli(int sock)
{
    int ret;
    struct sockaddr_in servaddr;
    struct sockaddr_in peeraddr;
    int peerlen = sizeof(peeraddr);
    char recvbuf[1024] = {0};

    if(sock < 0)
        return;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind failed");
        return;
    }
	fprintf(stdout, "serveraddr's address %p %lu\n", &servaddr, sizeof(servaddr));

    run_flag = 1;
    signal(SIGINT, sig_handle);
    while (run_flag){
        memset(recvbuf, 0, sizeof(recvbuf));
        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        if (ret < 0){
//            fprintf(stdout, ".");
            fflush(stdout);
        }

        fputs(recvbuf, stdout);
        if(0 == strncmp(recvbuf, "quit", strlen("quit")))
            run_flag = 0;
    }
}

int main(void)
{
    int sock;
    struct timeval timeout = {2, 0};

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");
	fprintf(stdout, "socket success[%d]\n", sock);

    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    echo_cli(sock);

    fprintf(stdout, "Program will exit\n");
    close(sock);
    return 0;
}
