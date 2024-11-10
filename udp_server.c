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

static int g_udps_sfd = -1;
int get_udps_sfd()
{
    return g_udps_sfd;
}
void set_udps_sfd(int sfd)
{
    g_udps_sfd = sfd;
}

int g_is_udps_connected = 0;
void set_udps_connected()
{
    g_is_udps_connected = 1;
}
void set_udps_disconnected()
{
    g_is_udps_connected = 0;
}

int is_udps_connected()
{
    return g_is_udps_connected;
}

int udp_server_init()
{
    int ret, sfd;
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
        close(sfd);
        return -1;
    }

    set_udps_sfd(sfd);
    return 0;
}


void* udp_server_rx_thread(void *arg)
{
    int sfd = get_udps_sfd();
    int rcv_len, ret = 0;
    struct sockaddr_in cliaddr;
    socklen_t addr_len;
    static char rcv_buf[1500];

    if (sfd < 0) {
        printf("sfd < 0!!!, return\n");
        return NULL;
    }

    addr_len = sizeof(cliaddr);

    rcv_len = recvfrom(sfd, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr *)&cliaddr, &addr_len);
    printf("recvfrom return len: %d\n", rcv_len);

    ret = connect(sfd, (const struct sockaddr *)&cliaddr, addr_len);
    if (ret < 0) {
        perror("connect error");
        return NULL;
    }

    set_udps_connected();

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

    set_udps_disconnected();
    return NULL;
}


void* udp_server_tx_thread(void *arg)
{
    int ret, sfd;
    static char snd_buf[1400];

    while (1) {
        if (!is_udps_connected()) {
            sleep(1);
            continue;
        }

        sfd = get_udps_sfd();

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

#if 0
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

