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
#include <ncurses.h>

#define PORT 80
#define BUFFER_SIZE 1024
#define ENTER_KEY 10

WINDOW *user_input, *chat_window;

void* handle_server(void* arg) {
  int sock_fd = *(int*)arg;
  char buffer[BUFFER_SIZE] = {0};
  int read_val;

  while ((read_val = read(sock_fd, buffer, BUFFER_SIZE)) > 0) {
    // printf("%s\n", buffer);
    mvwprintw(chat_window, 1, 1,"%s\n", buffer);
    wrefresh(chat_window);
    memset(buffer, 0, BUFFER_SIZE);
  }
  pthread_exit(NULL);
}

static void init_ncurses(void) {
	initscr();
	cbreak();
  clear();
	//noecho();

  int x_max, y_max;
  getmaxyx(stdscr, y_max, x_max);

  user_input = newwin(3, x_max-12, y_max-5, 5);
  box(user_input, 0, 0);
  wrefresh(user_input);
  keypad(user_input, true);

  chat_window = newwin(y_max-8, x_max-12, 3, 5);
  box(chat_window, 0, 0);
  wrefresh(chat_window);
}

void end_ncurses(void) {
  getch();
  endwin();
}

int main(int argc, char const *argv[]) {
  int sock_fd;
  struct sockaddr_in server_address;

  init_ncurses();
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  //server_address.sin_addr.s_addr = inet_addr("167.99.229.132");
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
    mvwprintw(user_input, 1, 1, ">");
    wrefresh(user_input);
    wgetstr(user_input, msg_from_client);

    if(strncmp(msg_from_client, "/exit\n", BUFFER_SIZE) == 0){
      puts("exiting....");
      end_ncurses();
      exit(0);
    }

    int send_val = send(sock_fd, msg_from_client, strlen(msg_from_client), 0);
    //wclear(user_input);
    if (send_val < 0) {
      perror("Sending failure");
      exit(EXIT_FAILURE);
    }
  }

  close(sock_fd);
  return 0;
}
