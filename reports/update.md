#Sofysys Server Client Update
##Shruti Iyer, Nora Mohamed, Lucy Wilcox
### Overview and Goals
Originally, the goal of the project was to better understand TCP networking. We were originally basing off what we were planning to do on a previous Software Systems team that made a [HTTP Server in C](https://matthewruehle.github.io/SoftSysQuestingQuail/)., where we would explore TCP, UDP, and HTTPS networking. We then thought that the project would be more worthwhile and engaging when we had the idea to make an instant messaging application. The IM application should:
 
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
 
### Current status
Currently, we are working on messaging functionality. When messages are sent, they have random characters attached to the end of them. We are working on removing those characters for a cleaner interface. This will be done when messages are sent without those extra characters. Nora is working on this.
 
Messages should be sent to everyone, so we are working on making sure messages from one client are sent to another (as opposed to just the server receiving them). We will know when this works when messages can be relayed from different clients. Currently, Shruti is working on this.
 
We are also working on making a sort of “log-in” system. Whenever a user is connected to the server, they should also be prompted to provide their identification. At the very least, a username so when messages are passed through the server from one client to another, the sender can be identified (whether it’s just their IP address or an actual username they input at the beginning). Lucy is working on this.

