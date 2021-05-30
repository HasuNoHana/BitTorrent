#ifndef TIN_SOCKET_H
#define TIN_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include <malloc.h>

int createSocket(int port, struct in6_addr clientAddress);

void closeSocket(int socketDescriptor);

void listenToConnect(int socketDescriptor);

int acceptConnection(int socketDescriptor, struct sockaddr_in6 client);

int closeConnection(int socketDescriptor);

int connectToDifferentSocket(int socketDescriptor, struct sockaddr_in6 server);

int sendData(int socketDescriptor, char *buf, int dataSize);

int receiveData(int socketDescriptor, int dataSize, char* data);

#endif

