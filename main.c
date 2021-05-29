#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/socket.h"
#include "include/user.h"
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 4

pthread_mutex_t writemsg_lock[MODULE_COUNT];

char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
int msg_read[MODULE_COUNT] = {0, 0, 0, 0};
int msg_write[MODULE_COUNT] = {0, 0, 0, 0};

typedef struct address_args{
    struct in6_addr clientAddress;
    struct in6_addr trackerAddress;
} AddressArgs;


int writemsg(int ID, char *message) {
    pthread_mutex_lock(writemsg_lock + ID);

    if ((msg_write[ID] == msg_read[ID] - 1 || msg_write[ID] == msg_read[ID] + MSG_ARRAY_SIZE - 1 % MSG_ARRAY_SIZE))
        return -1;

    strcpy(msg_array[ID][msg_write[ID]], message);

    msg_write[ID] = (msg_write[ID] + 1) % MSG_ARRAY_SIZE;

    pthread_mutex_unlock(writemsg_lock + ID);

    return 0;
}

int readmsg(int ID, char *buffer) {
    if (msg_read[ID] == msg_write[ID])
        return -1;

    int msg_read_old = msg_read[ID];
    msg_read[ID] = (msg_read[ID] + 1) % MSG_ARRAY_SIZE;

    strcpy(buffer, msg_array[ID][msg_read_old]);

    return 0;
}

void *super_createListenSocket(struct in6_addr clientAddress)
{
    while(1)
    {
        struct sockaddr_in6 address;

        int socketToListen = createSocket(3001, clientAddress);
        listenToConnect(socketToListen);

        struct sockaddr_in6 client;
        acceptConnection(socketToListen, client);


        char buf6[INET6_ADDRSTRLEN];
        struct in6_addr check_address = client.sin6_addr;
        inet_ntop(AF_INET6, &check_address, buf6, sizeof(buf6));

        printf("Accepted connection from %s\n", buf6);
        closeSocket(socketToListen);
        printf("Closed the socket associated with %s\n", buf6);
        //pobiera dane jaki będzie to plik
        //pobiera plik z jakiejś funkcji (?)
        //wysyła plik i wraca do słuchania
    }
}

void *super_createConnectSocket(struct in6_addr clientAddress)
{

    int numberOfQueueToRead = 1;
    int numberOfQueueToWrite = 2;
    int newSocketId;
    int port = 3030;

    struct sockaddr_in6 tracker;
    tracker.sin6_addr = clientAddress;     //TODO fix: clientAddress -> trackerAddress
    tracker.sin6_port = 8080;
    tracker.sin6_family = AF_INET6;

    while(1)
    {
        char *buffer = "4aaa";
//        if (readmsg(numberOfQueueToRead, buffer) == 0)
        {
            char option = *buffer;

            //koniec słuchania, zamykamy wszystko
            if(option == '0')
                break;

            buffer++;
            newSocketId = createSocket(port, clientAddress);
            //jeśli połaczenie do trackera się powiodło
            if (connectToDifferentSocket(newSocketId, tracker) == 0)
            {
                sendDataToDifferentUser(newSocketId, &option, 1, true);
                char *data = getDataFromDifferentUser(newSocketId, true);

                if(option == '1' || option == '2')  //dodanie lub usunięcie pliku (info do trackera)
                {
                    if (data != NULL)
                        sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                }

                else if(option == '3') //pobranie listy peerów z określonym plikiem
                {
                    if (data != NULL)
                    {
                        if (sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true) != -1)
                        {
                            char *listData = getDataFromDifferentUser(newSocketId, true);
                            writemsg(numberOfQueueToWrite, listData);
                        }
                    }
                }

                else if(option == '4') //pobierz dane do konkretnego peeru
                {
                    buffer++;
                    newSocketId = createSocket(port, clientAddress);

                    struct sockaddr_in6 test_local;
                    test_local.sin6_addr = clientAddress;
                    test_local.sin6_port = 3001;
                    test_local.sin6_family = AF_INET6;

                    if (connectToDifferentSocket(newSocketId, test_local) == 0)
                        closeSocket(newSocketId);
                    //łączy się ze wskazanym peerem na adresie 3000
                    //pobiera dane
                    //wysyła do trackera info, że ma taki plik
                }

                else break;

                closeSocket(newSocketId);
                break;
            }
        }
    }
}

void *socketSupervisorModule(struct in6_addr clientAddress)
{

    pthread_t listensocket_thread_id;
    pthread_t connectsocket_thread_id;

    int mode = 0;
    printf("Please input application mode (0 for server, 1 for client): ");
    scanf("%d", &mode);

    //do nasłuchiwania połączeń
    if(mode == 0)
    {
        if(pthread_create(&listensocket_thread_id, NULL, super_createListenSocket, (void *)&clientAddress))
        {
            printf("Failed to create ListenSocket\n");
            exit(1);
        }
    }

    //do obsługi kolejki i nawiązywania połączeń z innymi peerami
    if(mode == 1)
    {
        /*
        AddressArgs args;
        args.clientAddress = clientAddress;
        args.trackerAddress = trackerAddress;
        */

        if(pthread_create(&connectsocket_thread_id, NULL, super_createConnectSocket, (void *)&clientAddress))
        {
            printf("Failed to create ConnectSocket\n");
            exit(1);
        }
    }
}

int main()
{
    for (int i = 0; i < MODULE_COUNT; i++)
    {
        if (pthread_mutex_init(writemsg_lock + i, NULL) != 0)
        {
            printf("\n mutex #%i init failed\n", i);
            return 1;
        }
    }

    pthread_t supervisor_thread_id;
    struct sockaddr_in6 clientAddress;

    inet_pton(AF_INET6, "127.0.0.1", &(clientAddress.sin6_addr));

    if(pthread_create(&supervisor_thread_id, NULL, socketSupervisorModule, (void *)&clientAddress))
    {
        printf("Failed to create ConnectSocket\n");
        exit(1);
    }

    return 0;
}

