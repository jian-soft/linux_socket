

int udp_server_init();
void* udp_server_rx_thread(void *arg);
void* udp_server_tx_thread(void *arg);



int udp_client_init();
void* udp_client_rx_thread(void *arg);
void* udp_client_tx_thread(void *arg);


int socket_set_rcvtimeo(int sfd, int seconds);

void stat_inc_rx_bytes(int n);
void* stat_rx_speed_thread(void *arg);




