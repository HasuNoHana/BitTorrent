#include "../include/socket.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include <malloc.h>

int createSocket() {
    int socketDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
    if (socketDescriptor == -1) {
        puts("Could not create socket\n");
    }
    puts("Socket created\n");
    return socketDescriptor;
}

void closeSocket(int socketDescriptor) {
    close(socketDescriptor);
    puts("Socket closed\n");
}

void listenToConnect(int socketDescriptor) {
    //liczba requestów do ewentualnej zmiany
    listen(socketDescriptor, 5);
    puts("Listening started\n");
}

int acceptConnection(int socketDescriptor, struct sockaddr_in client) {
    int c = sizeof(struct sockaddr_in);

    int client_sock = accept(socketDescriptor, (struct sockaddr *) &client, (socklen_t *) &c);
    if (client_sock < 0) {
        perror("accept failed\n");
        return 1;
    }
    puts("Connection accepted\n");
}

int connectToDifferentSocket(int socketDescriptor, struct sockaddr_in server) {
    if (connect(socketDescriptor, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Connect failed. Error\n");
        return 1;
    }
    puts("Connected\n");
    return 0;
}

int sendData(int socketDescriptor, char *buf, int dataSize){
    if (send(socketDescriptor, buf, dataSize, 0) < 0) {
        puts("Send failed\n");
        return 1;
    } else {
        puts("Data sent\n");
        return 0;
    }
}

char *receiveData(int socketDescriptor, int dataSize) {
    char *data = malloc(dataSize);
    if(!data)
        return NULL;

    if( recv(socketDescriptor , data , dataSize , 0) < 0)
    {
        puts("Error while receiving data!\n");
        return NULL;
    }else{
        return data;
    }

}
