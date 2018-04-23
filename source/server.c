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

Client_info* make_client_info(struct sockaddr_in client_address, int sock_fd) {
  Client_info* new_client = (Client_info*) malloc(sizeof(Client_info));
  new_client->client_address = client_address;
  new_client->sock_fd = sock_fd;
  return new_client;
}

void add_client(Client_info* new_client) {
  if (total_clients < MAX_CLIENTS) {
    clients_array[total_clients] = new_client;
    total_clients = total_clients+1;
    char msg[23];
    snprintf(msg, sizeof msg, "New client joined - #%i", new_client->sock_fd);
    send_to_all_clients(msg);
  }
}

void send_to_all_clients(char msg[]){
  for(int i=0; i<total_clients; i++) {
    send(clients_array[i]->sock_fd, msg, strlen(msg), 0);
  }
}

void* handle_client(void* arg){
  int child_fd = *(int*)(arg);
  int read_val;
  char buffer[BUFFER_SIZE] = {0};

  while((read_val = read(child_fd, buffer, BUFFER_SIZE-1)) > 0){
    // Possible TODO: Determining who sent the message
    printf("Client #%i says: %s", child_fd, buffer);
    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof msg, "Client #%i says: %s", child_fd, buffer);
    send_to_all_clients(msg);
    memset(buffer, 0, BUFFER_SIZE);
  }
  pthread_exit(NULL);
}

// void* broadcast(void* arg){
//   while(1){
//     int child_fd = *(int*)(arg);
//     char writebuffer[BUFFER_SIZE] = {0};
//     printf("> ");
//     fgets(writebuffer, BUFFER_SIZE, stdin);
//
//     if(strncmp(writebuffer, "/exit\n", BUFFER_SIZE) == 0){
//       puts("Exiting now");
//       exit(0);
//     }
//
//     // send(child_fd, writebuffer, strlen(writebuffer), 0);
//     // send_to_all_clients(writebuffer);
//   }
//   pthread_exit(NULL);
// }

void* broadcast(void* arg){
  while(1){
    int child_fd = *(int*)(arg);
    char writebuffer[BUFFER_SIZE] = {0};
    printf("> ");
    fgets(writebuffer, BUFFER_SIZE, stdin);

    if(strncmp(writebuffer, "/exit\n", BUFFER_SIZE) == 0){
      puts("Exiting now");
      exit(0);
    }

    // send(child_fd, writebuffer, strlen(writebuffer), 0);
    // send_to_all_clients(writebuffer, snl_server);
    printf("Total clients %d\n", total_clients);
    for(int i=0; i<total_clients; i++) {
      send(clients_array[i], writebuffer, strlen(writebuffer), 0);
    }
  }
  close(child_fd);
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  int parent_fd, child_fd;
  int opt = 1;      // Reuse flag
  struct sockaddr_in server_address, client_address;
  pthread_t tidc, tidb;
  int retc, retb;

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

    retc = pthread_create(&tidc, NULL, &handle_client, (void*)&child_fd);
    if (retc != 0) {
      perror("pthread_create failed");
    }
    // retb = pthread_create(&tidb, NULL, &broadcast, (void*)&child_fd);
    // if (retb != 0) {
    //   perror("pthread_create failed");
    // }
    sleep(1);
  }
  close(child_fd);
  return 0;
}
