#ifndef TIN_SOCKET_H
#define TIN_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int createSocket();

void closeSocket(int socketDescriptor);

void listenToConnect(int socketDescriptor);

int acceptConnection(int socketDescriptor, struct sockaddr_in client);

int connectToDifferentSocket(int socketDescriptor, struct sockaddr_in server);

int sendData(int socketDescriptor, char *buf, int dataSize);

char *receiveData(int socketDescriptor, int dataSize) ;

#endif
