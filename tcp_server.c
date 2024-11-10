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

static int g_tcps_sfd = -1;
int get_tcps_sfd()
{
    return g_tcps_sfd;
}
void set_tcps_sfd(int sfd)
{
    g_tcps_sfd = sfd;
}

static int g_tcps_client_fd = -1;
int get_tcps_client_sfd()
{
    return g_tcps_client_fd;
}
void set_tcps_client_sfd(int sfd)
{
    g_tcps_client_fd = sfd;
}


int g_is_tcps_connected = 0;
void set_tcps_connected()
{
    g_is_tcps_connected = 1;
}
void set_tcps_disconnected()
{
    g_is_tcps_connected = 0;
}

int is_tcps_connected()
{
    return g_is_tcps_connected;
}

int tcp_server_init()
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
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(TCP_SERVER_PORT);

    ret = bind(sfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        perror("bind failed");
        close(sfd);
        return -1;
    }

    set_tcps_sfd(sfd);
    return 0;
}

void* tcp_server_listen_thread(void *arg)
{
    int sfd = get_tcps_sfd();
    int client_sfd, ret;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    if (sfd < 0) {
        printf("sfd < 0!!!, return\n");
        return NULL;
    }

RECONNECT:
    ret = listen(sfd, 3);
    if (ret < 0) {
        perror("listen");
        return NULL;
    }

    client_sfd = accept(sfd, (struct sockaddr*)&clientaddr, &addrlen);
    if (client_sfd < 0) {
        perror("accept");
        return NULL;
    }

    set_tcps_client_sfd(client_sfd);
    set_tcps_connected();

    while (1) {
        sleep(3);
        if (!is_tcps_connected()) {
            break;
        }
    }

    goto RECONNECT;

    return NULL;
}



void* tcp_server_rx_thread(void *arg)
{
    int sfd;
    int rcv_len, ret = 0;
    static char rcv_buf[1500];

    while (!is_tcps_connected()) {
        sleep(1);
    }

    sfd = get_tcps_client_sfd();

    while (1) {
        rcv_len = recv(sfd, rcv_buf, sizeof(rcv_buf), 0);
        if (rcv_len < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recv error");
                break;
            }
        }

        //printf("recv return len: %d\n", rcv_len);
    }

    set_tcps_disconnected();
    return NULL;
}


void* tcp_server_tx_thread(void *arg)
{
    int ret, sfd;
    static char snd_buf[1400];

    while (1) {
        if (!is_tcps_connected()) {
            sleep(1);
            continue;
        }

        sfd = get_tcps_client_sfd();

        while (1) {
            ret = send(sfd, snd_buf, 1400, 0);
            if (ret < 0) {
                perror("send error");
                break;
            }

            //printf("send return: %d\n", ret);
            //usleep(1000);  //1400B 1ms: 5.7Mbps
        }
    }

    return NULL;
}



