#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"


void* udp_server_rx_thread(void *arg)
{
    int sfd;
    int rcv_len, ret = 0;
    struct sockaddr_in cliaddr;
    socklen_t addr_len;
    static char rcv_buf[1500];

    addr_len = sizeof(cliaddr);

    //create udp server:
    sfd = udp_server_init();
    if (sfd < 0) {
        printf("udp_server_init fail. exit\n");
        return NULL;
    }

    while (1) {
        rcv_len = recvfrom(sfd, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr *)&cliaddr, &addr_len);
        printf("recvfrom return len: %d\n", rcv_len);

        rcv_len = sendto(sfd, rcv_buf, rcv_len, 0, (struct sockaddr *)&cliaddr, addr_len);
        printf("sendto return len: %d\n", rcv_len);
    }

    return NULL;
}



int main(int argc, char argv[])
{



    printf("Usage: ./main -u -s -p 1000\n");


    udp_server_rx_thread(NULL);



    return 0;
}



