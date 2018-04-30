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
#define BUFFER_SIZE 1024
int main(int argc, char const *argv[]) {

  char buffer[BUFFER_SIZE] = "/send 942442";
  char file_size_str[BUFFER_SIZE - 6];

  // sprintf(file_size_str, "%s", &buffer + 6);
  strncpy(file_size_str, buffer + 6, BUFFER_SIZE);
  int file_size = (int) strtol(file_size_str, NULL, 10);
  printf("File size: *%i*", file_size);
  return 0;
}
