#include "common.h"

int main(int argc, char **argv)
{
    struct sockaddr_in server_addr;
    char buffer[BUF_MAX_LEN + 1];
    int sockfd;
    int len;
    int ret;
    int port;

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

    bzero(buffer, BUF_MAX_LEN + 1);
    len = recv(sockfd, buffer, BUF_MAX_LEN, 0);
    if(len > 0) 
        printf("Recv msg successfully[%d]\n", len);
    else
        perror("Recv msg failed");

    bzero(buffer, BUF_MAX_LEN + 1);
    strcpy(buffer, "Test msg: client send to server\n");
    len = send(sockfd, buffer, strlen(buffer), 0);
    if(len < 0) 
        perror("send failed");
    else 
        printf("Send msg successfully[%d]\n", len);

out:
    close(sockfd);
    return 0;
}
