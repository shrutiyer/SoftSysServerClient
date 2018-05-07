# SoftSys Server Client Update

#### Shruti Iyer, Nora Mohamed, Lucy Wilcox

### Overview and Goals

Originally, the goal of the project was to better understand TCP networking. We were originally basing our project on a previous Software Systems project: [HTTP Server in C](https://matthewruehle.github.io/SoftSysQuestingQuail/). In this project we would explore TCP, UDP, and HTTPS networking. After that, we thought that our project would be more worthwhile and engaging by making an instant messaging application. The IM application should:
 
- Initially be based off of a TCP server
- Not require a GUI and will be run through bash
- Have a server that receives messages from different clients
- Pass messages from one client to another
- The server should maintain a list of all those who are connected to it with user names
 
As part of this project we want to learn about different layers of the network and understand how they are implemented. Generally we are more interested in expanding our knowledge about network protocols and how they work than creating a user friendly system. This might also mean that we do a substantial amount of reading on the topic of networking. We also want to continue to improve our C language skills and understand C libraries used in networking.
 
### Resources and Inspiration
To set up our TCP server we used "Beej's Guide to Network Programming" by Brian Hall  which contains resources to understand networking concepts and how to implement them using C. We also used Chapter 11 of Head First C which talks about sockets and networking.
 
We also reference three implementations of a TCP server/client pair:
 
[Geeks for Geeks](https://www.geeksforgeeks.org/socket-programming-cc/)

[Carnegie Mellon University](https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpserver.c)

[Lawrence Angrave server](https://github.com/angrave/SystemProgramming/wiki/Networking,-Part-4:-Building-a-simple-TCP-Server) and [client](https://github.com/angrave/SystemProgramming/wiki/Networking,-Part-3:-Building-a-simple-TCP-Client)

We are using [yorickdewid’s chat server](https://github.com/yorickdewid/Chat-Server) as an example of a chat server build in C.

### What got done

SNL Chat features one chat room where users (currently up to 10) can send and receive between each other. The server is hosted on an Ubuntu machine via a Digital Ocean droplet. A user can join the chat room if they know the server IP address (though it’s in our client file so this shouldn’t be a problem). Once they user runs the client code, they are ready to send to and receive messages from the chat room. The commands `/help` tells them all the information they need to participate in the chat room. After joining the room, the users have the option to set a different username for themselves using `/name`. Once they are done, they can use `/exit` to leave the room.

When a new client joins the server, the server records the identifying details about the client, like the client address and the file descriptor for the socket. For each client that joins, a new thread gets created to handle all of the client’s needs from the server.

```c
    Client_info* new_client = make_client_info(client_address, child_fd);
    // Add a new child fd
    add_client(new_client);

    retc = pthread_create(&tidc, NULL, &handle_client, (void*)new_client);
    if (retc != 0) {
          perror("pthread_create failed");
}
```
The function, `handle_client`, handles two important features: allows users to change names and passes the message to other clients.

Since different types of messages might need to send information to different groups of users, we made three functions. `send_to_all_clients` sends messages to all the clients currently active, including the sender. `send_to_other_clients` sends messages to everyone but the sender. `send_to_new_client` sends information to welcome a new client to the chat room.

```c
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
```

On the client's side, once the client/server connection is established, a new thread is created to receive and print all messages from other clients and the server. This allows us to separate sending and receiving messages.

We tried to create a user interface that keeps the input window at the bottom of the screen separate from the message window. The client starts the `ncurses` window that creates two separate windows to create the UX. Whenever a new message gets sent, it appears in the top window. When the client is typing a message, they enter it in the bottom window.


### Reflection

Originally, we had two primary goals: create a TCP server and add an FTP layer. Our stretch goals were to create HTTP server/client communication, improve usability of our upload/download system, and add functionality for searching for files on the server. However, soon after proposing the project we pivoted to instead work on a messaging client. This did require a TCP server, but had no real reason to include an FTP layer and had little relevance to our stretch goals. So instead, we aimed to complete the goals listed in the first section of this report. All of the goals listed above were completed. We also used `ncurses` to create a more user friendly interface, and attempted to add a file sharing aspect to the project. Ncurse works kind of like expected; there is no scroll functionality yet, and sometimes exiting the program can create visual issues. We did get far into creating a way to send files to the server, and it technically did work. There were some issues with how the server received the files (since multiple messages had to be depending on the file size), resulting in formatting errors. This file sharing code can be seen in the `file` branch of our repository.

Our first learning goals were to expand our knowledge about network protocols and how they work rather than creating a user friendly system. We fulfilled both aspects of this goal. An example of this is learning about network address translations which is how computers on a shared network that don’t communicate with the outside world via their real IP can communicate with and get messages back from these outside computers. We didn’t learn very much about production level code, but we did learn more about C! For example, we learned a bit about string manipulation.

Here is an example in `server.c` of getting a substring.

```
      char* old_client_name = malloc(sizeof(char)*USERNAME_SIZE);
      strcpy(old_client_name, client->name);
      char* client_name = malloc(sizeof(char)*USERNAME_SIZE);
      strcpy(client_name, buffer);
      client_name = client_name + 6;
```
Along the way, we also learned about different libraries to help us improve the application. One of them is `ncurses`, which allowed us to write text-based user interfaces in any terminal environment. We learned about how to draw windows, update them with the messages we received from other clients, and erase the contents. We never found one place to learn all the details about `ncurses`; we referred to 3 different websites to get all details about the library. This is the main reason why learning and implementing the UX in the application took time. Adding the `ncurses` library enhanced our user interface and provided a better way for users to use the SNL Chat. 
