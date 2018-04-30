all: server.c
	make server client packages

server: server.c
	gcc -Wall -o server server.c -lpthread

client: client.c
	gcc -Wall -o client client.c -lpthread -lncurses

packages:
	apt-get install libncurses5-dev libncursesw5-dev
