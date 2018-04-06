#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define PORT 3000

int main(int argc, char const *argv[]) {
  int parent_fd, child_fd;
  int opt = 1;      // Reuse flag
  struct sockaddr_in server_address;

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

  int server_address_len = sizeof(server_address);
  child_fd = accept(parent_fd, (struct sockaddr *)&server_address, &server_address_len);
  if (child_fd < 0) {
    perror("Accepting error");
    exit(EXIT_FAILURE);
  }

  // Possible TODO: Determining who sent the message
  char buffer[BUFFER_SIZE];
  char *msg_from_server = "Hello From The Server";
  int read_val = read(child_fd, buffer, BUFFER_SIZE);
  if (read_val < 0) {
    perror("Reading error");
    exit(EXIT_FAILURE);
  }

  printf("From Client: %s\n", buffer);
  send(child_fd, msg_from_server, strlen(msg_from_server), 0);
  close(child_fd);

  return 0;
}
