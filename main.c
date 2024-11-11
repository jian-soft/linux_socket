#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //getopt
#include <string.h>  //strdup
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include "socket.h"


int run_udp_server()
{
    int ret;
    pthread_t thread_rx, thread_tx;

    ret = udp_server_init();
    if (ret < 0) {
        printf("udp_server_init fail. exit\n");
        goto OUT;
    }

    ret = pthread_create(&thread_rx, NULL, udp_server_rx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_tx, NULL, udp_server_tx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    pthread_join(thread_rx, NULL);
    pthread_join(thread_tx, NULL);

OUT:
    return -1;
}

int run_udp_client(char *serverip)
{
    int ret;
    pthread_t thread_rx, thread_stat;

    if (!serverip) {
        goto OUT;
    }

    ret = udp_client_init();
    if (ret < 0) {
        printf("udp_client_init fail. exit\n");
        goto OUT;
    }
    ret = pthread_create(&thread_rx, NULL, udp_client_rx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_stat, NULL, stat_rx_speed_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    pthread_join(thread_rx, NULL);
    pthread_join(thread_stat, NULL);

OUT:
    return -1;

}

int run_tcp_server()
{
    int ret;
    pthread_t thread_rx, thread_tx, thread_listen;

    ret = tcp_server_init();
    if (ret < 0) {
        printf("tcp_server_init fail. exit\n");
        goto OUT;
    }
    ret = pthread_create(&thread_listen, NULL, tcp_server_listen_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_rx, NULL, tcp_server_rx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_tx, NULL, tcp_server_tx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    pthread_join(thread_rx, NULL);
    pthread_join(thread_tx, NULL);
    pthread_join(thread_listen, NULL);

OUT:
    return -1;
}

int run_tcp_client(char *serverip)
{
    int ret;
    pthread_t thread_rx, thread_stat, thread_connect;

    ret = tcp_client_init(serverip);
    if (ret < 0) {
        printf("udp_client_init fail. exit\n");
        goto OUT;
    }
    ret = pthread_create(&thread_connect, NULL, tcp_client_connect_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_rx, NULL, tcp_client_rx_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    ret = pthread_create(&thread_stat, NULL, stat_rx_speed_thread, NULL);
    if (0 != ret) {
        printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
        goto OUT;
    }
    pthread_join(thread_connect, NULL);
    pthread_join(thread_rx, NULL);
    pthread_join(thread_stat, NULL);

OUT:
    return -1;
}

int main(int argc, char* argv[])
{
    char *opt_string = "utsc:";
    int opt;
    int udptcp = -1; //1-udp, 2-tcp
    int serverclient = -1; //1-server, 2-client
    char *serverip = NULL;

    while ((opt = getopt(argc, argv, opt_string)) != -1) {
        switch (opt) {
        case 'u':
            udptcp = 1;
            break;
        case 't':
            udptcp = 2;
            break;
        case 's':
            serverclient = 1;
            break;
        case 'c':
            serverclient = 2;
            printf("optarg: %s\n", optarg);
            serverip = strdup(optarg);
            break;
        default:
            printf("Usage: ./socket -u/t -s/-c serverip\n");
            exit(EXIT_FAILURE);
        }
    }

    if (udptcp < 0 || serverclient < 0) {
        printf("Not assign udp/tcp or server/client\n");
        printf("Usage: ./socket -u/t -s/-c serverip\n");
        exit(EXIT_FAILURE);
    }

    printf("Start %s %s\n", udptcp == 1 ? "udp" : "tcp",
                          serverclient == 1 ? "server" : "client");


    if (1 == udptcp && 1 == serverclient) {
        run_udp_server();
    } else if (1 == udptcp && 2 == serverclient) {
        run_udp_client(serverip);
    } else if (2 == udptcp && 1 == serverclient) {
        run_tcp_server();
    }else if (2 == udptcp && 2 == serverclient) {
        run_tcp_client(serverip);
    }

    if (!serverip) {
        free(serverip);
    }

    printf("program exit.\n");

    return 0;
}



