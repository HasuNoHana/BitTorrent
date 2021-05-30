#include "../include/socket.h"
#include <stdlib.h>
#include <arpa/inet.h>


int createSocket(int port, struct in6_addr userAddress) {
    int socketDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
    if (socketDescriptor == -1) {
        puts("Could not create socket\n");
        exit(1);
    }
    puts("Socket created\n");
    struct sockaddr_in6 address;
    address.sin6_family = AF_INET6;
    address.sin6_addr = userAddress;
    address.sin6_port = htons(port);
    if (bind(socketDescriptor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    return socketDescriptor;
}

void closeSocket(int socketDescriptor) {
    if (close(socketDescriptor) != 0) {
        perror("Could not close socket");
        exit(1);
    } else
        puts("Socket closed\n");
}

void listenToConnect(int socketDescriptor) {
    //liczba requestów do ewentualnej zmiany
    if (listen(socketDescriptor, 5) != 0) {
        perror("Listening failed");
        exit(1);
    }
    puts("Listening started\n");
}

int acceptConnection(int socketDescriptor, struct sockaddr_in6 client) {
    int c = sizeof(struct sockaddr_in);

    int client_sock = accept(socketDescriptor, (struct sockaddr *) &client, (socklen_t * ) & c);
    if (client_sock < 0) {
        perror("accept failed");
        exit(1);
    }
    puts("Connection accepted\n");
    return 0;
}

int closeConnection(int socketDescriptor) {
    //zwraca 0 gdy zamknięty poprawnie i -1 gdy error
    //drugi argument oznacza, że przestajemy zarówno wysyłac jak i odbierać dane
    return shutdown(socketDescriptor, 2);
}


int connectToDifferentSocket(int socketDescriptor, struct sockaddr_in6 server) {
    char buffer[64];
    inet_ntop(AF_INET6, &server.sin6_addr.s6_addr, buffer, sizeof(server));
    printf("Trying to connect to IP %s at port %d...\n", buffer, ntohs(server.sin6_port));

    if (connect(socketDescriptor, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Connect failed");
        exit(1);
    }
    puts("Connected\n");
    return 0;
}

int sendData(int socketDescriptor, char *buf, int dataSize) {
    int numberOfSent = send(socketDescriptor, buf, dataSize, 0);
    if (numberOfSent < 0) {
        puts("Send failed\n");
        return 1;
    } else {
        puts("Data sent\n");
        return numberOfSent;
    }
}

int receiveData(int socketDescriptor, int dataSize, char* data) {
    int numberOfRead = recv(socketDescriptor, data, dataSize, 0);
    if (numberOfRead < 0) {
        puts("Error while receiving data!\n");
        return -1;
    }else {
        return numberOfRead;
    }

}

