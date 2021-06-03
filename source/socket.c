#include "../include/socket.h"
#include <stdlib.h>
#include <arpa/inet.h>


int createSocket(int port, struct in6_addr userAddress) {
    int socketDescriptor = socket(AF_INET6, SOCK_STREAM, 0);
    if (socketDescriptor == -1) {
        puts("Could not create socket\n");
        exit(1);
    }
    int opt = 1;
    if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("[-] Setsockopt() error.\n");
        exit(EXIT_FAILURE);
    }
    puts("Socket created\n");
    struct sockaddr_in6 address;
    address.sin6_family = AF_INET6;
    address.sin6_addr = userAddress;
    address.sin6_port = htons(port);
    if (bind(socketDescriptor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed.\n");
        exit(1);
    }
    return socketDescriptor;
}

void closeSocket(int socketDescriptor) {
    if (close(socketDescriptor) != 0) {
        perror("Could not close socket.\n");
        exit(1);
    } else
        puts("Socket closed.\n");
}

void listenToConnect(int socketDescriptor) {
    //liczba requestów do ewentualnej zmiany
    if (listen(socketDescriptor, 5) != 0) {
        perror("Listening failed.\n");
        exit(1);
    }
    puts("Listening started.\n");
}

int acceptConnection(int socketDescriptor, struct sockaddr_in6 client) {
    int c = sizeof(struct sockaddr_in);

    int client_sock = accept(socketDescriptor, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_sock < 0) {
        perror("Accept failed.\n");
        return -1;
    }
    puts("Connection accepted.\n");
    return client_sock;
}

int closeConnection(int socketDescriptor) {
    //zwraca 0 gdy zamknięty poprawnie i -1 gdy error
    //drugi argument oznacza, że przestajemy zarówno wysyłac jak i odbierać dane
    return shutdown(socketDescriptor, 2);
}


int connectToDifferentSocket(int socketDescriptor, struct sockaddr_in6 server) {
    char buffer[64];
    inet_ntop(AF_INET6, &server.sin6_addr.s6_addr, buffer, sizeof(server));
    printf("Trying to connect to IP %s at port %d...\n\n", buffer, ntohs(server.sin6_port));

    if (connect(socketDescriptor, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Connect failed.");
        exit(1);
    }
    puts("Connected.\n");
    return 0;
}

int sendData(int socketDescriptor, char *buf, int dataSize) {
    int numberOfSent = write(socketDescriptor, buf, dataSize);
    if (numberOfSent < 0) {
        puts("Send failed.\n");
        return 1;
    } else {
        puts("Data sent.\n");
        return numberOfSent;
    }
}

int receiveData(int socketDescriptor, int dataSize, char data[1024]) {
    return read(socketDescriptor, data, dataSize);
//    printf("Receive data %d\n",numberOfRead );
//   for(int i = 0; i <1024 ; ++i){
//
//        printf(" sssss %d\n",data[numberOfRead-1] );
//   }
//    puts("\n");
//    return numberOfRead;
}

void sendFileToTracker(FILE *fp, int client_socket){
    char* data = malloc(1024);
    while(fgets(data, 1024, fp) != NULL) {
        int writeNumber = send(client_socket, data, 1024, 0);
        if (writeNumber == -1){
            perror("[-] Error in sending file.");
            exit(1);
        }
        bzero(data, 1024);
    }
    return;
}

