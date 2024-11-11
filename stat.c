#include <string.h>
#include <unistd.h>
#include <stdio.h>


static unsigned int g_rx_bytes;
static unsigned int g_rx_pkts;

void stat_inc_rx_bytes(int n)
{
    g_rx_bytes += n;
    g_rx_pkts++;
}


void* stat_rx_speed_thread(void *arg)
{
    unsigned int last_rx_bytes;
    unsigned int delta_rx_bytes;
    unsigned int last_rx_pkts;
    unsigned int delta_rx_pkts;

    while (1) {
        last_rx_bytes = g_rx_bytes;
        last_rx_pkts = g_rx_pkts;
        sleep(1);
        delta_rx_bytes = g_rx_bytes - last_rx_bytes;
        delta_rx_pkts = g_rx_pkts - last_rx_pkts;
        printf("rx speed: %ukbps, %upkt\n", (delta_rx_bytes<<3)/1000, delta_rx_pkts);
    }

    return NULL;
}



