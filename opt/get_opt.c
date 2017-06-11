#include "../comm/comm.h"

static void printUsage(void)
{
    fprintf(stdout, "---------------------------------------------\n");
    fprintf(stdout, " cmd tcp\t\tGet sock opts of TCP\n");
    fprintf(stdout, " cmd udp\t\tGet sock opts of UDP\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Support opts: SOCK_SNDBUF|SOCK_RCVBUF\n");
    fprintf(stdout, "---------------------------------------------\n");
}

int main(int argc, const char *argv[])
{
    int sock = -1;
    int sndBuf = 0;
    int rcvBuf = 0;
    socklen_t optlen;
    int ret = -1;

    fprintf(stdout, "\nBuild time: %s %s\n", __DATE__, __TIME__);
    if(2 != argc){
        fprintf(stdout, "Param incorrent\n");
        printUsage();
        return 0;
    }
    
    if(0 == strcasecmp("-h", argv[1])){
        printUsage();
        return 0;
    }else if(0 == strcasecmp("tcp", argv[1])){
        sock = socket(AF_INET, SOCK_STREAM, 0);
    }else if(0 == strcasecmp("udp", argv[1])){
        sock = socket(AF_INET, SOCK_DGRAM , 0);
    }else{
        printUsage();
        return 0;
    }

    if(-1 == sock){
        perror("socket failed");
        return 0;
    }
    optlen = sizeof(sndBuf);
    ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sndBuf, &optlen);
    if(-1 == ret){
        perror("get sockopt failed");
        goto out;
    }

    optlen = sizeof(rcvBuf);
    ret = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBuf, &optlen);
    if(-1 == ret){
        perror("get sockopt failed");
        goto out;
    }

    fprintf(stdout, "socket: %d\n", sock);
    fprintf(stdout, "Send buf %d\n", sndBuf);
    fprintf(stdout, "Recv buf %d\n", rcvBuf);

out:
    close(sock);
    return ret;
}
