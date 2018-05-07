# SoftSysServerClient
We created BashChat, an application that allows people to talk within a chatroom.

In order to run the the whole application, follow these steps.

1. Find out the IPv4 address of the machine running the server.
      ```
      user@machine:~$ ip addr show
      ```

1. Edit [this](https://github.com/shrutiyer/SoftSysServerClient/blob/master/source/client.c#L108) line in client.c to point to the server IP address.

1. Run `make` (if needed `sudo make`) in the client machine. This will install the packages needed for `ncurses` library and compile the code.

1. Run the server and client executables in their respective machines. You might need to run it with `sudo` since we are using port 80, which is usually reserved for HTTP.

1. Chat away!
