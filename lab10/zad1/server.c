#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define MAX_MESSAGE_LENGTH 256
#define MAX_CLIENTS 16
pthread_mutex_t clientsInfoMutex = PTHREAD_MUTEX_INITIALIZER;

struct Client
{
	char *name;
	int fd;
	int isActive;
};

int clientsNo;  
struct Client *clients[MAX_CLIENTS+1] ={NULL};

char name[20];
char buffer[MAX_MESSAGE_LENGTH + 1];
char command[200];
char argument[200];
pthread_t thrd;
int localSocket;
int webSocket;

int reciveFromBoth(int localSocket, int webSocket);
void initilizeSockets(int *localSocket, int *webSocket, const char *port, const char *path);
int interpret();
int opponentNo(int i);
void registerPlayer(const char *name);
void forwardMoveInfo(const char *name, const char *move);
void removeClient(int clientId, char *msg);
int getByName(const char *name);
void denyRegistration(const char *name, int clientId);
void ping();
void farewell(int clientFd, char *message);
void registerClient(const char *name, int fd);
int listnenMessages(int localSocket, int webSocket);
void quit();

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,"Invalid argument number error.");
		exit(-1);
	}
	srand(time(NULL));
	char webAddres[20], localPath[20];
	strcpy(webAddres,argv[1]);
	strcpy(localPath,argv[2]);

	
	initilizeSockets(&localSocket, &webSocket, webAddres, localPath);
	int currentSocket;
	clientsNo = 0;
	signal(SIGINT,quit);

	pthread_create(&thrd, NULL, (void *(*)(void *))ping, NULL);
	while (1)
	{
		currentSocket = listnenMessages(localSocket, webSocket);
		recv(currentSocket, buffer, MAX_MESSAGE_LENGTH, 0);
		pthread_mutex_lock(&clientsInfoMutex);
		printf("x%sx\n", buffer);
		interpret();

		if (strcmp(command, "add") == 0)
		{
			registerClient(name, currentSocket);
		}

		else if (strcmp(command, "move") == 0)
			forwardMoveInfo(name, argument);

		else if (strcmp(command, "ping") == 0)
		{
			int clientID = getByName(name);
			if (clientID != -1)
				clients[clientID]->isActive = 1;
		}
		else if (strcmp(command, "quit") == 0)
			removeClient(getByName(name), "Koniec rozgrywki !");
		else
		{
			fprintf(stderr, "SOCKET ERROR TRY AGAIN.\n");
		}
		pthread_mutex_unlock(&clientsInfoMutex);
	}

	return 0;
}

void quit()
{
	
	printf("Quiting.\n");
	for (int i=0;i<MAX_CLIENTS;i++)
	{
		if (clients[i]!=NULL)	removeClient(i,"Koniec rozgrywki ! Wylaczono serwer!\n");
	}
	shutdown(localSocket,SHUT_RDWR);
	shutdown(webSocket,SHUT_RDWR); 
	close(localSocket);
	close(webSocket);
	pthread_join(thrd,NULL);
	exit(0);
}

void ping()
{
	while (1)
	{
		//printf("Sending Pings.\n");
		pthread_mutex_lock(&clientsInfoMutex);

		for (int i = 0; i < MAX_CLIENTS; i++)
			if (clients[i] != NULL && !clients[i]->isActive)
				removeClient(i, "Run timeout error!\n");

		for (int i = 0; i < MAX_CLIENTS; i++)
			if (clients[i] != NULL)
			{
				char *ptr;
				sprintf(ptr, "ping %d", i);
				send(clients[i]->fd, ptr, MAX_MESSAGE_LENGTH, 0);
				clients[i]->isActive = 0;
			}
		pthread_mutex_unlock(&clientsInfoMutex);
		sleep(3);
	}
}

void initilizeSockets(int *localSocket, int *webSocket, const char *port, const char *path)
{
	//intializing local connection
	*localSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un localSockaddr;
	memset(&localSockaddr, 0, sizeof(struct sockaddr_un));
	localSockaddr.sun_family = AF_UNIX;
	strcpy(localSockaddr.sun_path, path);
	unlink(path);
	bind(*localSocket, (struct sockaddr *)&localSockaddr,
		 sizeof(struct sockaddr_un));
	listen(*localSocket, MAX_CLIENTS);


	//intinalizing webconnection
	struct sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(atoi(port));
    address.sin_addr.s_addr=INADDR_ANY;
	*webSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(*webSocket, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
	listen(*webSocket, MAX_CLIENTS / 2);
}

int listnenMessages(int localSocket, int webSocket)
{
	struct pollfd *fds = malloc((2 + clientsNo) * sizeof(struct pollfd));
	fds[0].fd = localSocket;
	fds[0].events = POLLIN;
	fds[1].fd = webSocket;
	fds[1].events = POLLIN;
	int k = 0;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i] == NULL)
			continue;
		fds[2 + k].fd = clients[i]->fd;
		fds[2 + k].events = POLLIN;
		k++;
	}
	poll(fds, clientsNo + 2, -1);
	int retval;
	// check for message
	for (int i = 0; i < clientsNo + 2; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			retval = fds[i].fd;
			break;
		}
	}
	if (retval == localSocket || retval == webSocket)
	{
		retval = accept(retval, NULL, NULL);
	}
	free(fds);
	return retval;
}

void forwardMoveInfo(const char *name, const char *move)
{
	printf("Forwarding message %s to %s opponent\n", move, name);
	char message[MAX_MESSAGE_LENGTH + 1];
	strcpy(message, "move ");
	strcat(message, move);

	int clientId = getByName(name);
	if (clientId == -1)
	{
		fprintf(stderr, "Klient wciaz gra mimo, ze drugi wyszedl. \n");
		removeClient(clientId, "Koniec gry!\n");
		return;
	}
	int opponentId = opponentNo(clientId);
	send(clients[opponentId]->fd, message, MAX_MESSAGE_LENGTH, 0);
}

int interpret()
{
	char *ptr = strtok(buffer, " ");
	if (ptr == NULL)
		return 0;
	strcpy(name, ptr);

	ptr = strtok(NULL, " ");
	if (ptr == NULL)
		return 0;

	strcpy(command, ptr);

	ptr = strtok(NULL, " ");
	if (!(ptr == NULL))
		strcpy(argument, ptr);

	return 1;
}

int opponentNo(int i)
{
	if (i % 2 == 0)
		return i + 1;
	return i - 1;
}

void registerClient(const char *name, int fd)
{
	printf("Registering client %s \n", name);
	int clientId = getByName(name);
	if (clientId != -1)
	{
		farewell(fd, "quit Name already taken!");
		return;
	}

	int index = -1;

	for (int i = 0; i < MAX_CLIENTS; i += 2) //w pierwszej kolejnosci wybieramy miejsca z brakujaca para
	{
		if (clients[i] != NULL && clients[i + 1] == NULL)
		{
			index = i + 1;
			break;
		}
	}
	if (index == -1)
	{
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients[i] == NULL)
			{
				index = i;
				break;
			}
		}
	}

	struct Client *newClient = malloc(sizeof(struct Client));
	newClient->name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	strcpy(newClient->name, name);
	newClient->fd = fd;
	newClient->isActive = 1;
	clientsNo++;

	clients[index] = newClient;

	if (index % 2 == 1)
	{
		printf("Found pair! client %s and %s\n", name, clients[index - 1]->name);
		if (rand() % 2 == 0)
		{
			send(clients[index]->fd, "add O", MAX_MESSAGE_LENGTH, 0);
			send(clients[index - 1]->fd, "add X", MAX_MESSAGE_LENGTH, 0);
		}
		else
		{
			send(clients[index]->fd, "add X", MAX_MESSAGE_LENGTH, 0);
			send(clients[index - 1]->fd, "add O", MAX_MESSAGE_LENGTH, 0);
		}
	}
	else
	{
		send(fd, "add no_enemy", MAX_MESSAGE_LENGTH, 0);
	}
}

void removeClient(int clientId, char *msg)
{
	if (clientId == -1)
		return;

	char message[MAX_MESSAGE_LENGTH + 1];
	strcpy(message, "quit ");
	strcat(message, msg);

	farewell(clients[clientId]->fd, message);
	free(clients[clientId]);
	clientsNo--;

	int opponentId = opponentNo(clientId);


	if (clients[opponentId] != NULL)
	{
		clientsNo--;
		farewell(clients[opponentId]->fd, message);
		free(clients[opponentId]);
	}

	clients[clientId] = NULL;
	clients[opponentId] = NULL;
}

int getByName(const char *name)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
			return i;

	return -1;
}
void farewell(int clientFd, char *message)
{
	send(clientFd, message, MAX_MESSAGE_LENGTH, 0);
	shutdown(clientFd,SHUT_RDWR);
	close(clientFd);
}