#include "common.h"

void sig_handler(int sig)
{
    fprintf(stdout, "Get a signal[%d]\n", sig);
    run_flag = 0;
}

int main(int argc, char **argv)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int port;
    unsigned int num;
    char buf[BUF_MAX_LEN + 1];
    int sockfd;
    int new_fd;
    int ret;
    socklen_t len;
   
    if(1 == argc){
        port = DEFAULT_PORT;
        num = DEFAULT_QUEUE_LENGTH;
    }else if(2 == argc){
        port = atoi(argv[1]);
        num = DEFAULT_QUEUE_LENGTH;
    }else if(3 == argc){
        port = atoi(argv[1]);
        num = atoi(argv[2]);
    }else{
        fprintf(stdout, "Usage: cmd port queue-length\n");
        return 0;
    }
    fprintf(stdout, "local port %d queue length %d\n", port, num);
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        perror("socket failed");
        return 0;
    }
    printf("Socket created successfully\n");

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(port);
//    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_addr.s_addr = INADDR_ANY;
   
    ret =  bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
    if (-1 == ret) {
        perror("bind failed");
        goto out;
    }
    fprintf(stdout, "Bind successfully\n");
   
    ret =  listen(sockfd, num);
    if (-1 == ret) {
        perror("listen failed");
        goto out;
    }
    printf("Waiting for client...\n");

    setSockTimeout(sockfd, 2);
    signal(SIGINT, sig_handler);
    run_flag = 1;    
    while(run_flag) {
        len = sizeof(struct sockaddr);
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (-1 == new_fd) {
            fprintf(stdout, ".");
            fflush(stdout);
            continue;
        }
        fprintf(stdout, "Get a client:[%s:%d]\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        bzero(buf, BUF_MAX_LEN + 1);
        strcpy(buf, "Test msg: server send to client\n");
        len = send(new_fd, buf, strlen(buf), 0);
        if(len < 0) {
            perror("send failed");
        }
        else{ 
            printf("Send msg successfully[%d]\n", len);
        }
 
        bzero(buf, BUF_MAX_LEN + 1);
        len = recv(new_fd, buf, BUF_MAX_LEN, 0);
        if((int)len > 0)
            printf("Recv msg successfully[%d]\n", len);
        else 
            perror("recv failed");
    }

out:
    close(sockfd);
    return 0;
}
