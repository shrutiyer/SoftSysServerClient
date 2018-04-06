#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
  int sock_fd;
  struct sockaddr_in server_address;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);

  int connect_val = connect(sock_fd, (const struct sockaddr *)&server_address, sizeof(server_address));
  if (connect_val < 0) {
    perror("Connection failure");
    exit(EXIT_FAILURE);
  }

  char *msg_from_client = "Hello From Client";
  int send_val = send(sock_fd, msg_from_client, strlen(msg_from_client), 0);
  if (send_val < 0) {
    perror("Sending failure");
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];
  int read_val = read(sock_fd, buffer, BUFFER_SIZE);
  printf("From Server: %s\n", buffer);
  close(sock_fd);

  return 0;
}
