/*
  SNL (Shruti, Nora, Lucy) Server Client Project
  Created for Software Systems Project 2
  Authors: LucyWilcox, nmohamed, shrutiyer

  This is a bash-based chat client.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 80
#define BUFFER_SIZE 1024

/*
Thread that handles server and prints incoming messages
Inputs: int* of socket file descriptor cast as a a void*
Returns: Nothing
*/
void* handle_server(void* arg) {
  int sock_fd = *(int*)arg;
  char buffer[BUFFER_SIZE] = {0};
  int read_val;

  while ((read_val = read(sock_fd, buffer, BUFFER_SIZE)) > 0) {

    printf("%s\n", buffer);
    memset(buffer, 0, BUFFER_SIZE);
  }
  pthread_exit(NULL);
}

/*
Main function, connects to server, waits for user input and sends to server
Input: argc, argv
Returns: 0 when done
*/
int main(int argc, char const *argv[]) {
  int sock_fd;
  struct sockaddr_in server_address;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(server_address)); // clear address
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr("192.168.33.215"); //ip of server
  server_address.sin_port = htons(PORT);

  int connect_val = connect(sock_fd, (const struct sockaddr *)&server_address, sizeof(server_address));
  if (connect_val < 0) {
    perror("Connection failure");
    exit(EXIT_FAILURE);
  }
  // we make a new thread for reading from the server
  pthread_t tid;
  pthread_create(&tid, NULL, &handle_server, (void*)&sock_fd);
  while(1) {
    char msg_from_client[BUFFER_SIZE] = {0};
    printf("> ");
    fgets(msg_from_client, BUFFER_SIZE, stdin);
    if(strncmp(msg_from_client, "/exit\n", BUFFER_SIZE) == 0){
      puts("exiting....");
      exit(0);
    } else if(strncmp(msg_from_client, "/help\n", BUFFER_SIZE) == 0){
      puts("~~~~~~~~~~~~~~~");
      puts("~Welcome to the SNL chatroom~");
      puts("To...");
      puts("Exit type /exit");
      puts("Change your username type /name your_new_username");
      puts("~~~~~~~~~~~~~~~");
    } else{
      int send_val = send(sock_fd, msg_from_client, strlen(msg_from_client), 0);
      if (send_val < 0) {
        perror("Sending failure");
        exit(EXIT_FAILURE);
      }
    }
  }

  close(sock_fd);
  return 0;
}
