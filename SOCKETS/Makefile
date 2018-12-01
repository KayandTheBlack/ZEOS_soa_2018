all: serverSocket clientSocket launchClient serverSocketThreads

launchClient: launchClient.c
	gcc -o launchClient launchClient.c

serverSocket: serverSocket.c socketMng.o
	gcc -o serverSocket -g serverSocket.c socketMng.o

serverSocketThreads: serverSocketThreads.c socketMng.o
	gcc -pthread -o serverSocketThreads -g serverSocketThreads.c socketMng.o


clientSocket: clientSocket.c socketMng.o
	gcc -o clientSocket -g clientSocket.c socketMng.o

socketMng.o: socketMng.c
	gcc -c -g socketMng.c

clean: 
	rm -f serverSocket clientSocket socketMng.o launchClient

cleanLogs:
	rm client_* launch_info
