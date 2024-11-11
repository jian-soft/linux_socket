#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"

#define UDP_SERVER_PORT 19999


static int g_udpc_sfd = -1;
int get_udpc_sfd()
{
    return g_udpc_sfd;
}
void set_udpc_sfd(int sfd)
{
    g_udpc_sfd = sfd;
}

int g_is_udpc_connected = 0;
void set_udpc_connected()
{
    g_is_udpc_connected = 1;
}
void set_udpc_disconnected()
{
    g_is_udpc_connected = 0;
}

int is_udpc_connected()
{
    return g_is_udpc_connected;
}

int udp_client_init()
{
    int ret, sfd;
    socklen_t addr_len;
    struct sockaddr_in servaddr;

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd <= 0) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(UDP_SERVER_PORT);
    addr_len = sizeof(servaddr);

    ret = connect(sfd, (const struct sockaddr *)&servaddr, addr_len);
    if (ret < 0) {
        perror("socket connect failed");
        close(sfd);
        return -1;
    }

    set_udpc_sfd(sfd);
    return 0;
}


void* udp_client_rx_thread(void *arg)
{
    int sfd = get_udpc_sfd();
    int rcv_len, ret = 0;
    static char rcv_buf[1500];

    if (sfd < 0) {
        printf("sfd < 0!!!, return\n");
        return NULL;
    }

    socket_set_rcvtimeo(sfd, 2);
    while (1) {
        ret = send(sfd, rcv_buf, 10, 0);
        if (ret < 0) {
            perror("send error");
        }

        rcv_len = recv(sfd, rcv_buf, sizeof(rcv_buf), 0);
        if (rcv_len > 0) {
            printf("rcv msg from server, connected, len:%d\n", rcv_len);
            set_udpc_connected();
            break;
        }
    }

    socket_set_rcvtimeo(sfd, 0);
    while (1) {
        rcv_len = recv(sfd, rcv_buf, sizeof(rcv_buf), 0);
        if (rcv_len < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recv error");
            }
        }

        //printf("recv return len: %d\n", rcv_len);
        stat_inc_rx_bytes(rcv_len);
    }

    return NULL;
}


void* udp_client_tx_thread(void *arg)
{
    int ret, sfd;
    static char snd_buf[1400];

    while (1) {
        if (!is_udpc_connected()) {
            sleep(1);
            continue;
        }
    }

    sfd = get_udpc_sfd();
    while (1) {
        ret = send(sfd, snd_buf, 1000, 0);
        if (ret < 0) {
            perror("send error");
        }

        printf("send return: %d\n", ret);
        sleep(1);
    }
}



