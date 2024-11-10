

int udp_server_init();
void* udp_server_rx_thread(void *arg);
void* udp_server_tx_thread(void *arg);

int udp_client_init();
void* udp_client_rx_thread(void *arg);
void* udp_client_tx_thread(void *arg);


int tcp_server_init();
void* tcp_server_listen_thread(void *arg);
void* tcp_server_rx_thread(void *arg);
void* tcp_server_tx_thread(void *arg);


int tcp_client_init();
void* tcp_client_rx_thread(void *arg);
void* tcp_client_tx_thread(void *arg);



int socket_set_rcvtimeo(int sfd, int seconds);

void stat_inc_rx_bytes(int n);
void* stat_rx_speed_thread(void *arg);




