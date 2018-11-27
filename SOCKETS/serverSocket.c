#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#define SEQSERV 0
#define CONSERV 1
#define BOUNDSERV 2
#define THREADSERV 3

int forksAlive = 0;

doService(int fd) {
    int i = 0;
    char buff[80];
    char buff2[80];
    int ret;
    int socket_fd = (int) fd;

	ret = read(socket_fd, buff, sizeof(buff));
	while(ret > 0) {
		buff[ret]='\0';
		sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
		write(1, buff2, strlen(buff2));
		ret = write(fd, "caracola ", 8);
		if (ret < 0) {
			perror ("Error writing to socket");
			exit(1);
		}
		ret = read(socket_fd, buff, sizeof(buff));
	}
	if (ret < 0) {
			perror ("Error reading from socket");

	}
	sprintf(buff2, "Server [%d] ends service\n", getpid());
	write(1, buff2, strlen(buff2));
}


doServiceFork(int fd) {
    int ret=-1;
    while(ret<0) ret = fork();
    forksAlive++;
    if(ret == 0) {
        doService(fd);
        pthread_exit();
    }
}
void proc_exit() {
    waitpid(-1, NULL);
    forksAlive--;
}

main (int argc, char *argv[])
{
    int socketFD;
    int connectionFD;
    char buffer[80];
    int ret;
    int port;
    int serverType;
    int maxChild;

    if (argc < 3)
    {
      strcpy (buffer, "Usage: ServerSocket PortNumber ServerType (Bound)\n");
      write (2, buffer, strlen (buffer));
      exit (1);
    }
    
    serverType = atoi(argv[2]);
    signal (SIGCHLD, proc_exit);
    if(serverType == SEQSERV) {
        port = atoi(argv[1]);
        socketFD = createServerSocket (port);
        if (socketFD < 0){
          perror ("Error creating socket\n");
          exit (1);
        }

        while (1) {
          connectionFD = acceptNewConnections (socketFD);
          if (connectionFD < 0) {
	          perror ("Error establishing connection \n");
	          deleteSocket(socketFD);
	          exit (1);
          }

          doService(connectionFD);
        }
    } else if(serverType == CONSERV) {
    // Need to use signals *surprised pikachu*
        port = atoi(argv[1]);
        socketFD = createServerSocket (port);
        if (socketFD < 0){
          perror ("Error creating socket\n");
          exit (1);
        }

        while (1) {
          connectionFD = acceptNewConnections (socketFD);
          if (connectionFD < 0) {
	          perror ("Error establishing connection \n");
	          deleteSocket(socketFD);
	          exit (1);
          }
          doServiceFork(connectionFD);
        }
    } else if(serverType == BOUNDSERV) {
        if (argc < 4)
        {
          strcpy (buffer, "Usage: ServerSocket PortNumber 2 Bound\n");
          write (2, buffer, strlen (buffer));
          exit (1);
        }
        port = atoi(argv[1]);
        maxChild = atoi(argv[3]);
        socketFD = createServerSocket (port);
        if (socketFD < 0){
          perror ("Error creating socket\n");
          exit (1);
        }

        while (1) {
          connectionFD = acceptNewConnections (socketFD);
          if (connectionFD < 0) {
	          perror ("Error establishing connection \n");
	          deleteSocket(socketFD);
	          exit (1);
          }
          while(forksAlive>=maxChild);
          doServiceFork(connectionFD);
        }
    } /*else if(serverType == THREADSERV){
            port = atoi(argv[1]);
            socketFD = createServerSocket (port);
            if (socketFD < 0){
              perror ("Error creating socket\n");
              exit (1);
            }

            while (1) {
              connectionFD = acceptNewConnections (socketFD);
              if (connectionFD < 0) {
	              perror ("Error establishing connection \n");
	              deleteSocket(socketFD);
	              exit (1);
              }

              doService(connectionFD);
            }
        }
    } */else {
        perror("ServType should be between 0 and 3 \n");
    }
}
