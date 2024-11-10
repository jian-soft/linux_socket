#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //getopt
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include "socket.h"


int main(int argc, char* argv[])
{
    char *opt_string = "utsc";
    int opt, ret;
    int udptcp = -1; //1-udp, 2-tcp
    int serverclient = -1; //1-server, 2-client
    pthread_t thread_rx, thread_tx, thread_stat, thread_listen;

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
            break;
        default:
            printf("Usage: ./socket -u/t -s/c\n");
            exit(EXIT_FAILURE);
        }
    }

    if (udptcp < 0 || serverclient < 0) {
        printf("Not assign udp/tcp or server/client\n");
        printf("Usage: ./socket -u/t -s/c\n");
        exit(EXIT_FAILURE);
    }

    printf("Start %s %s\n", udptcp == 1 ? "udp" : "tcp",
                          serverclient == 1 ? "server" : "client");

    if (1 == udptcp && 1 == serverclient) {
        ret = udp_server_init();
        if (ret < 0) {
            printf("udp_server_init fail. exit\n");
            exit(EXIT_FAILURE);
        }

        ret = pthread_create(&thread_rx, NULL, udp_server_rx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_tx, NULL, udp_server_tx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        pthread_join(thread_rx, NULL);
        pthread_join(thread_tx, NULL);
    } else if (1 == udptcp && 2 == serverclient) {
        ret = udp_client_init();
        if (ret < 0) {
            printf("udp_client_init fail. exit\n");
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_rx, NULL, udp_client_rx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_stat, NULL, stat_rx_speed_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        pthread_join(thread_rx, NULL);
        pthread_join(thread_stat, NULL);
    } else if (2 == udptcp && 1 == serverclient) {
        ret = tcp_server_init();
        if (ret < 0) {
            printf("tcp_server_init fail. exit\n");
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_listen, NULL, tcp_server_listen_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_rx, NULL, tcp_server_rx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_tx, NULL, tcp_server_tx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        pthread_join(thread_rx, NULL);
        pthread_join(thread_tx, NULL);
        pthread_join(thread_listen, NULL);
    }else if (2 == udptcp && 2 == serverclient) {
        ret = tcp_client_init();
        if (ret < 0) {
            printf("udp_client_init fail. exit\n");
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_rx, NULL, tcp_client_rx_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        ret = pthread_create(&thread_stat, NULL, stat_rx_speed_thread, NULL);
        if (0 != ret) {
            printf("pthread_create return:%d, line:%d\n", ret, __LINE__);
            exit(EXIT_FAILURE);
        }
        pthread_join(thread_rx, NULL);
        pthread_join(thread_stat, NULL);
    }

    printf("program exit.");

    return 0;
}



