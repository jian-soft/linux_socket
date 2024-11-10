#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"

#define TCP_SERVER_PORT 19999


static int g_tcpc_sfd = -1;
int get_tcpc_sfd()
{
    return g_tcpc_sfd;
}
void set_tcpc_sfd(int sfd)
{
    g_tcpc_sfd = sfd;
}

static int g_is_tcpc_connected = 0;
void set_tcpc_connected()
{
    g_is_tcpc_connected = 1;
}
void set_tcpc_disconnected()
{
    g_is_tcpc_connected = 0;
}
int is_tcpc_connected()
{
    return g_is_tcpc_connected;
}

int tcp_client_init()
{
    int ret, sfd;
    struct sockaddr_in servaddr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd <= 0) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(TCP_SERVER_PORT);

    ret = connect(sfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        perror("socket connect failed");
        close(sfd);
        return -1;
    }

    set_tcpc_sfd(sfd);
    set_tcpc_connected();
    return 0;
}


void* tcp_client_rx_thread(void *arg)
{
    int sfd = get_tcpc_sfd();
    int rcv_len, ret = 0;
    static char rcv_buf[1500];

    if (sfd < 0) {
        printf("sfd < 0!!!, return\n");
        return NULL;
    }

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


void* tcp_client_tx_thread(void *arg)
{
    int ret, sfd;
    static char snd_buf[1400];

    while (1) {
        if (!is_tcpc_connected()) {
            sleep(1);
            continue;
        }
    }

    sfd = get_tcpc_sfd();
    while (1) {
        ret = send(sfd, snd_buf, 1000, 0);
        if (ret < 0) {
            perror("send error");
        }

        printf("send return: %d\n", ret);
        sleep(1);
    }
}




