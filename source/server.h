#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define PORT 80
#define MAX_CLIENTS 10

/* Source: https://github.com/yorickdewid/Chat-Server/blob/master/chat_server.c*/
typedef struct {
	struct sockaddr_in client_address;	/* Client remote address */
	int sock_fd;			      /* Socket file descriptor */
} Client_info;

Client_info* make_client_info(struct sockaddr_in client_address, int sock_fd);
void add_client(Client_info* new_client);
void send_to_all_clients(char msg[]);
