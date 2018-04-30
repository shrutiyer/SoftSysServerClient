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
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_port = htons(PORT);

  int connect_val = connect(sock_fd, (const struct sockaddr *)&server_address, sizeof(server_address));
  if (connect_val < 0) {
    perror("Connection failure");
    exit(EXIT_FAILURE);
  }

  pthread_t tid;
  pthread_create(&tid, NULL, &handle_server, (void*)&sock_fd);
  while(1) {
    char msg_from_client[BUFFER_SIZE] = {0};
    printf("> ");
    fgets(msg_from_client, BUFFER_SIZE, stdin);

	  // Exit from client
    if(strncmp(msg_from_client, "/exit\n", BUFFER_SIZE) == 0){
      puts("exiting....");
      exit(0);
    }

	  // Send file
  	if(strncmp(msg_from_client, "/send ", 6) == 0){
  		char* file_name = malloc(sizeof(char)*BUFFER_SIZE-6);
  		strcpy(file_name, msg_from_client);
  		file_name = file_name + 6;
  		file_name[strlen(file_name)-1] = 0;
      // printf("Sending %s...\n", file_name);

  		// Open file
  		FILE *file;
  		file = fopen(file_name, "r");
  		fseek(file, 0, SEEK_END);
  		int size = ftell(file);
  		fseek(file, 0, SEEK_SET);

  		// Send size
      char* size_message = malloc(sizeof(size)+6);
      sprintf(size_message, "/send %i", size);
  		int send_val = send(sock_fd, size_message, strlen(size_message), 0);
  		if (send_val < 0) {
  	      perror("Sending failure");
  	      exit(EXIT_FAILURE);
  	  }

      // Denote completion

  		// Send file
  		char file_buffer[size];
  		while(!feof(file)) {
        puts("* ");
        fread(file_buffer, 1, sizeof(file_buffer), file);
        send_val = send(sock_fd, file_buffer, strlen(file_buffer), 0);
        bzero(file_buffer, sizeof(file_buffer));
  			if (send_val < 0) {
  				perror("Sending failure");
  				exit(EXIT_FAILURE);
  			}
      }
      
      char completion_message[20] = "send complete";
      send_val = send(sock_fd, completion_message, strlen(completion_message), 0);
      if (send_val < 0) {
          perror("Sending failure");
          exit(EXIT_FAILURE);
      }

      fclose(file);
      continue;
    }

    int send_val = send(sock_fd, msg_from_client, strlen(msg_from_client), 0);
    if (send_val < 0) {
      perror("Sending failure");
      exit(EXIT_FAILURE);
    }
  }

  close(sock_fd);
  return 0;
}
