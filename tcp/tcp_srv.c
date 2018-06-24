#include "tcp_srv.h"

#define DEBUG   1

static pthread_t srv_pids[SERVER_THREAD_MAX];

static int get_client_id(void)
{
    int i = 0;

    for(; i < SERVER_THREAD_MAX; i++){
        if(-1 == srv_pids[i]){
            return i;
        }
    }

    return -1;
}

static void *server_route(void *arg)
{
    char buf[BUF_MAX_LEN + 1];
    int len = -1;
    int max_fd = 0;
    fd_set rd_fds;
    struct timeval timeout;
    struct timeval t_end;
    unsigned int frame_rate = 0;
    unsigned long long last_time_sec = 0;

    if(NULL == arg)
        goto out;
    PThreadParam param = (PThreadParam)arg;
    if(param->sock_fd < 0)
        goto out;

    fprintf(stdout, "server for client[%d] is running\n", param->client_id);
    max_fd = param->sock_fd + 1;
   do{
       FD_ZERO(&rd_fds);
       FD_SET(param->sock_fd, &rd_fds);

       timeout.tv_sec = 5;
       timeout.tv_usec = 0;
       len = select(max_fd, &rd_fds, NULL, NULL, &timeout);
       if(-1 == len){
           perror("select failed: ");
           break;
       }else if(0 == len){
#if DEBUG
           fprintf(stdout, "Timeout, and try again\n");
#endif
           continue;
       }

        bzero(buf, BUF_MAX_LEN + 1);
        len = recv(param->sock_fd, buf, BUF_MAX_LEN, 0);

        if((int)len > 0){
#if DEBUG
            fprintf(stdout, "Get a msg: %s\n", buf);
#endif
        }else if(0 == len){ 
            fprintf(stdout, "Maybe client has been disconnected\n");
            break;
        }else{
            if(EINTR == errno){
                fprintf(stdout, "Get a signal\n");
                continue;
            }
#if DEBUG
            perror("recv failed: ");
#endif
            break;
        }

        frame_rate ++;
        if(0 == strncmp("quit", buf, 4)){
            break;
        }
#if DEBUG
        len = send(param->sock_fd, buf, strlen(buf) + 1, 0);
        if(len < 0) {
            perror("send failed");
        }
        else{ 
            printf("Send msg successfully[%d]\n", len);
        }
#endif
        gettimeofday(&t_end, NULL);
        if(t_end.tv_sec - last_time_sec > 1){
            fprintf(stdout, "Get msg's frame rate: %d\n", frame_rate);
            frame_rate = 0;
            last_time_sec = t_end.tv_sec;
        }
   }while(run_flag); 

   close(param->sock_fd);
   param->sock_fd = -1;
   srv_pids[param->client_id] = -1;
   fprintf(stdout, "Server for client[%d] will exit\n", param->client_id);
   free(param);

out:
    return NULL; 
}

int main(int argc, char **argv)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int port;
    unsigned int num;
    int sockfd;
    int new_fd;
    int ret;
    int i;
    socklen_t len;
    int client_id = -1;
    int opt = 1;
   
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
  
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(-1 == ret){
        perror("setsockopt fialed\n");
        goto out;
    }

    ret = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr));
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

    for(i = 0; i < SERVER_THREAD_MAX; i++)
        srv_pids[i] = -1;

    while(run_flag) {
        len = sizeof(struct sockaddr);
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (-1 == new_fd)
            continue;
        fprintf(stdout, "Get a client:[%s:%d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
   
        client_id = get_client_id();
        if(client_id < 0){
            fprintf(stdout, "THere is no resource for new thread\n");
            close(new_fd);
            continue;
        }

        PThreadParam param = (PThreadParam)malloc(sizeof(ThreadParam));
        if(NULL == param){
            fprintf(stdout, "THere is no resource for new thread2\n");
            close(new_fd);
            continue;
        }

        param->sock_fd = new_fd;
        param->client_id = client_id;

        ret = pthread_create(&srv_pids[client_id], NULL, server_route, (void*)param);
        if(0 != ret){
            fprintf(stdout, "Create thread failed.\n");
        }
    }

    for(i = 0; i < SERVER_THREAD_MAX && -1 != srv_pids[i]; i++)
        pthread_join(srv_pids[i], NULL);

out:
    close(sockfd);
    fprintf(stdout, "main thread will exit\n");
    return 0;
}
