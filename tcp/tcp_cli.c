#include "common.h"

int main(int argc, char **argv)
{
    struct sockaddr_in server_addr;
    char buffer[BUF_MAX_LEN + 1];
    int sockfd;
    int len;
    int ret;
    int port;

    fd_set rd_fds;
    struct timeval timeout;
    int max_fd = 0;

    if(3 == argc) {
        port = atoi(argv[2]);
    }else if(2 == argc){
        port = DEFAULT_PORT;
    }else{
        printf("Usage: cmd IP port\n");
        return 0;
    }
    fprintf(stdout, "Port is %d\n", port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket");
        return 0;
    }
    printf("Creat socket successfully\n");

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    ret = inet_aton(argv[1], (struct in_addr *) &server_addr.sin_addr.s_addr);
    if (0 == ret) {
        perror("inet_aton failed");
        goto out;
    }

    setSockTimeout(sockfd, 2);
    ret = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (0 != ret) {
        perror("Connect failed");
        goto out;
    }
    printf("Connect to server successfully\n");

    signal(SIGINT, sig_handler);
    run_flag = 1;
    do{
        bzero(buffer, BUF_MAX_LEN + 1);
        fprintf(stdout, "Input a string: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = 0x00;      // overwrite the '\n' char
        len = send(sockfd, buffer, strlen(buffer), 0);
        if(len < 0) 
            perror("send failed");
        else 
            printf("Send msg successfully[%d]\n", len);

        if(0 == strcmp("quit", buffer)){
            run_flag = 0;
            continue;
        }

        FD_ZERO(&rd_fds);
        FD_SET(sockfd, &rd_fds);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        max_fd = sockfd + 1;
        len = select(max_fd, &rd_fds, NULL, NULL, &timeout);
        if(-1 == len){
            perror("select failed: ");
            run_flag = 0;
            continue;
        }else if(0 == len){
            fprintf(stdout, "Timeout, and try again\n");
            continue;
        }

        bzero(buffer, BUF_MAX_LEN + 1);
        len = recv(sockfd, buffer, BUF_MAX_LEN, 0);
        if(len > 0){
            printf("Get a msg: %s\n", buffer);
        }else if(0 == len){
            fprintf(stdout, "Maybe server has been disconnected\n");
            run_flag = 0;
            continue;
        }else{
            if(EINTR == errno){
                fprintf(stdout, "Get a signal\n");
            }
            perror("Recv msg failed");
            run_flag = 0;
            continue;
        }
    }while(run_flag);

out:
    close(sockfd);
    return 0;
}
