/*
  SNL (Shruti, Nora, Lucy) Server Client Project
  Created for Software Systems Project 2
  Authors: LucyWilcox, nmohamed, shrutiyer

  This is a bash-based chat server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

Client_info* clients_array[MAX_CLIENTS];   // List of clients in the chat
int total_clients = 0;


/*
Creates a new client struct
Inputs: sockaddr stuct, int of the socket file descriptor
Returns: Client_info* of new client
*/
Client_info* make_client_info(struct sockaddr_in client_address, int sock_fd) {
  Client_info* new_client = (Client_info*) malloc(sizeof(Client_info));
  new_client->client_address = client_address;
  new_client->sock_fd = sock_fd;
  new_client->name = malloc(sizeof(char)*USERNAME_SIZE);
  sprintf(new_client->name, "%d", sock_fd); // set name to socket_fd for now
  return new_client;
}

/*
Adds client to client array
Inputs: Client_info* of client to be added
Returns: Nothing
*/
void add_client(Client_info* new_client) {
  if (total_clients < MAX_CLIENTS) {
    clients_array[total_clients] = new_client; // add client to array
    total_clients = total_clients+1; // increase number of clients
    char msg[23];
    snprintf(msg, sizeof msg, "New client joined - #%s\n", new_client->name);
    send_to_new_client(new_client->sock_fd);
    send_to_all_clients(msg);
  }
}

/*
Broadcasts message to all clients connected, for most special messages
Inputs: char[] of message to be sent
Returns: Nothing
*/
void send_to_all_clients(char msg[]){
  for(int i=0; i<total_clients; i++) {
    send(clients_array[i]->sock_fd, msg, strlen(msg), 0);
  }
}

/*
Broadcasts message to other clients connected, for most normal messages
Inputs: char[] of message to be sent
Returns: Nothing
*/
void send_to_other_clients(char msg[], int sender){
  for(int i=0; i<total_clients; i++) {
    if(clients_array[i]->sock_fd != sender){
      send(clients_array[i]->sock_fd, msg, strlen(msg), 0);
    }
  }
}

void send_to_new_client(int client_fd){
  char msg[BUFFER_SIZE];
  snprintf(msg, sizeof msg, "~Welcome to the SNL chatroom~\n");
  send(client_fd, msg, strlen(msg), 0);
  snprintf(msg, sizeof msg, "Type /help for commands\n");
  send(client_fd, msg, strlen(msg), 0);
}
/*
Code for handling each client thread
Inputs: Client_info* casts as void*
Returns: Nothing
*/
void* handle_client(void* arg){
  Client_info* client = (Client_info*)(arg);
  int child_fd = client->sock_fd;
  int read_val;
  char buffer[BUFFER_SIZE] = {0};

  while((read_val = read(child_fd, buffer, BUFFER_SIZE-1)) > 0){
    printf("Client #%i says: %s", child_fd, buffer);
    char msg[BUFFER_SIZE];

    // change username
    if(!strncmp(buffer, "/name", 5)){

      char* old_client_name = malloc(sizeof(char)*USERNAME_SIZE);
      strcpy(old_client_name, client->name);
      char* client_name = malloc(sizeof(char)*USERNAME_SIZE);
      strcpy(client_name, buffer);
      client_name = client_name + 6;
      client_name[strlen(client_name)-1] = 0;
      client->name = client_name;

      snprintf(msg, sizeof msg, "client %s is now client %s", old_client_name, client->name);
      send_to_all_clients(msg);

    } else{

      snprintf(msg, sizeof msg, "%s: %s", client->name, buffer);
      send_to_all_clients(msg, client->sock_fd);
    }
    memset(buffer, 0, BUFFER_SIZE);
  }
  pthread_exit(NULL);
}

/*
Main function, sets up server socket and listens, accepts clients
Inputs: argc argv
Returns: 0 when closed
*/
int main(int argc, char const *argv[]) {
  int parent_fd, child_fd;
  int opt = 1;      // Reuse flag
  struct sockaddr_in server_address, client_address;
  pthread_t tidc;
  int retc;

  parent_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (parent_fd == 0) {
    perror("Socket creation failed.");
    exit(EXIT_FAILURE);
  }

  // This forces the server to attach itself to the port, for immediate use.
  // Else, we would have to wait for 20 secs
  if (setsockopt(parent_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                  (const void *)&opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  memset((char *)&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(PORT);

  // Bind the socket to the port
  if (bind(parent_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("Binding failed");
    exit(EXIT_FAILURE);
  }

  // 10 is our queue length
  if (listen(parent_fd, 10) < 0) {
    perror("Listening error");
    exit(EXIT_FAILURE);
  }

  // Start accepting clients
  while(1){
    printf("Server online\n");
    int client_address_len = sizeof(client_address);
    child_fd = accept(parent_fd, (struct sockaddr *)&client_address, &client_address_len);
    if (child_fd < 0) {
      perror("Accepting error");
      exit(EXIT_FAILURE);
    }

    Client_info* new_client = make_client_info(client_address, child_fd);
    // Add a new child fd
    add_client(new_client);

    retc = pthread_create(&tidc, NULL, &handle_client, (void*)new_client);
    if (retc != 0) {
      perror("pthread_create failed");
    }

    sleep(1);
  }
  close(child_fd);
  return 0;
}
