#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define UDP_SERVER_PORT 19999



int udp_server_init()
{
    int ret;
    int sfd;
    struct sockaddr_in servaddr;

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd <= 0) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_SERVER_PORT);

    ret = bind(sfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        perror("bind failed");
        return -1;
    }

    return sfd;
}

#if 0
int socket_set_rcvtimeo(int sfd, int seconds)
{
    int ret;
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    ret = setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    if (0 != ret) {
        perror("setsockopt fail");
    }
    return ret;
}

int udp_server_wait_connection(int sfd)
{
    int rcv_len, ret = 0;
    struct sockaddr_in cliaddr;
    socklen_t addr_len;
    char rcv_buf[16];

    addr_len = sizeof(cliaddr);

    memset(&cliaddr, 0, addr_len);
    ret = connect(sfd, (const struct sockaddr *)&cliaddr, addr_len);
    if (ret < 0) {
        perror("connect error");
        return -1;
    }

    while (1) {
        rcv_len = recvfrom(sfd, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr *)&cliaddr, &addr_len);
        if (rcv_len <= 0) {
            perror("recvfrom error");
            ret = -1;
            break;
        }
        printf("recv in wait connection: len:%d\n", rcv_len);
        if (8 == rcv_len && 0 == strncmp(rcv_buf, "ucp:sync", 8)) {
            ret = connect(sfd, (const struct sockaddr *)&cliaddr, addr_len);
            if (ret < 0) {
                perror("connect error");
                break;
            }

            send(sfd, "ucp:ack", 7, 0);
            os_if_usleep(10000);
            break;
        }
    }

    return ret;
}
#endif

