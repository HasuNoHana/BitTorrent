#ifndef TIN_SOCKET_H
#define TIN_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>

//create socket, define address etc., return socket Descriptor
int createSocket();

//close socket
void closeSocket(int socketDescriptor);

int listenToConnect();

int acceptConnection(int socketDescriptor);

int connectToDifferentSocket(int socketDescriptor);

void sendData(int socketDescriptor, char *buf);

char *receiveData(int socketDescriptor);

#endif
