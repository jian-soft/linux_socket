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

static struct sockaddr_in g_serveraddr;

int tcp_client_init(char *serverip)
{
    struct sockaddr_in *serveraddr = &g_serveraddr;

    if (NULL == serverip) {
        return -1;
    }

    if (strcmp(serverip, "local") == 0) {
        serveraddr->sin_addr.s_addr = inet_addr("127.0.0.1");
    } else {
        if (inet_aton(serverip, &(serveraddr->sin_addr)) == 0) {
            printf("serverip is invalid: %s\n", serverip);
            return -1;
        }
    }

    serveraddr->sin_family = AF_INET;
    serveraddr->sin_port = htons(TCP_SERVER_PORT);

    return 0;
}

void* tcp_client_connect_thread(void *arg)
{
    int sfd;
    struct sockaddr_in *serveraddr = &g_serveraddr;

RECONNECT:
    sfd = get_tcpc_sfd();
    if (sfd > 0) {
        close(sfd);
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd <= 0) {
        perror("socket creation failed");
        return NULL;
    }

    while (connect(sfd, (const struct sockaddr *)serveraddr, sizeof(*serveraddr)) < 0) {
        perror("socket connect failed");
        sleep(3);
    }

    printf("TCP client connected.\n");
    set_tcpc_sfd(sfd);
    set_tcpc_connected();

    while (is_tcpc_connected()) {
        sleep(2);
    }

    goto RECONNECT;

    return NULL;
}

void* tcp_client_rx_thread(void *arg)
{
    int sfd;
    int rcv_len;
    static char rcv_buf[1500];

RETRY:
    while (!is_tcpc_connected()) {
        sleep(2);
    }

    sfd = get_tcpc_sfd();
    while (1) {
        rcv_len = recv(sfd, rcv_buf, sizeof(rcv_buf), 0);
        if (rcv_len < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recv error");
                break;
            }
        } else if (rcv_len == 0) {
            printf("rcv 0, disconnect\n");
            break;
        }

        stat_inc_rx_bytes(rcv_len);
    }

    set_tcpc_disconnected();

    goto RETRY;

    return NULL;
}

void* tcp_client_tx_thread(void *arg)
{
    int ret, sfd;
    static char snd_buf[1400];

RETRY:
    while (!is_tcpc_connected()) {
        sleep(2);
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

    set_tcpc_disconnected();

    goto RETRY;

    return NULL;
}




