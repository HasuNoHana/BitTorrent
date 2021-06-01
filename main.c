#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/socket.h"
#include "include/user.h"
#include <arpa/inet.h>

#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 4

pthread_mutex_t writemsg_lock[MODULE_COUNT];

char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
int msg_read[MODULE_COUNT] = {0, 0, 0, 0};
int msg_write[MODULE_COUNT] = {0, 0, 0, 0};


struct in6_addr trackerAddress;


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

long readFile(char filePath[40], char *buffer) {
    FILE *fp;
    long lSize;

    fp = fopen(filePath, "rb");
    if (!fp) {
        perror(filePath);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    buffer = calloc(1, lSize + 1);
    if (!buffer) {
        fclose(fp);
        fputs("memory alloc fails", stderr);
        exit(1);
    }

    if (1 != fread(buffer, lSize, 1, fp)) {
        fclose(fp);
        free(buffer);
        fputs("entire read fails", stderr);
        exit(1);
    }
    fclose(fp);
    return lSize;
}


void *listenSection(void *userAddr) {
    struct in6_addr userAddress = *(struct in6_addr *) userAddr;

    char buffer[64];
    inet_ntop(AF_INET6, &userAddress.s6_addr, buffer, sizeof(userAddress));
    printf("IP passed to ListenSocket: %s\n", buffer);

    while (1) {
        int opt = 1;
        char fileName[1024];

        int socketToListen = createSocket(3001, userAddress);
        listenToConnect(socketToListen);

        struct sockaddr_in6 client;
        if (acceptConnection(socketToListen, client) == 1) {
            break;
        } else {
            char buf6[INET6_ADDRSTRLEN];
            struct in6_addr check_address = client.sin6_addr;
            inet_ntop(AF_INET6, &check_address, buf6, sizeof(buf6));

            printf("Accepted connection from %s\n", buf6);

            getDataFromDifferentUser(socketToListen, fileName, true);

            //TODO: nie jestem pewna czy tu powinnam tego szukać (do dopytania u Zu)
            char folder[] = "../sharedFiles/";

            //odczytujemy nazwę pliku
            char name[40];
            int count = 0;
            char c;
            while ((c = *buffer) != '\n'){
                name[count] = c;
                count++;
            }
            char finalName[count];
            for(int i = 0; i< count; ++i){
                finalName[0] = name [0];
            }
            strcat(finalName, folder);

            char *buffer;
            long fileSize = readFile(folder, buffer);
            sendDataToDifferentUser(socketToListen, buffer, fileSize, false);

            //ponowne użycie socketu
            if (setsockopt(socketToListen, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                           &opt, sizeof(opt))) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void *queueSection(void *clientAddr) {

    struct in6_addr clientAddress = *(struct in6_addr *) clientAddr;

    int numberOfQueueToRead = 1;
    int numberOfQueueToWrite = 2;
    int newSocketId;
    int port = 3030;

    struct sockaddr_in6 tracker;
    tracker.sin6_addr = trackerAddress;
    tracker.sin6_port = htons(8080);
    tracker.sin6_family = AF_INET6;


    while (1) {
        char buffer[64];
        if (readmsg(numberOfQueueToRead, buffer) == 0)
        {
            char option = *buffer;

            //koniec słuchania, zamykamy wszystko
            if (option == '0') {
                break;
            }

            //przypadki gdy połączenie "idzie" do trackera
            if (option == '1' || option == '2' || option == '3') {

                //tworzymy nowy socket i bindujemy mu nasz adres
                newSocketId = createSocket(port, clientAddress);
                sendDataToDifferentUser(newSocketId, &option, 1, true);
                char resultData[1024];
                getDataFromDifferentUser(newSocketId, resultData, true);

                if (option == '1') {
                    if (resultData[0] == 'O' && resultData[1] == 'K') {
                        //rozmiar pliku torrent
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }
                        //liczymy rozmiar pliku
                        int count = 1;
                        char size[20];
                        int i = 0;
                        char c;
                        while ((c = *buffer) != '\n') {
                            size[i] = c;
                            count++;
                            i++;
                        }
                        int fileSize = 0;
                        i = 0;
                        for (int j = count; j >= 1; --j) {
                            fileSize += size[i] * 10 ^ j;
                            i++;
                        }

                        readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }

                        sendDataToDifferentUser(newSocketId, buffer, fileSize, true);
                    }
                } else if (option == '2') {
                    if (resultData[0] == 'O' && resultData[1] == 'K') {
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }
                        sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                    }
                } else {
                    if (resultData[0] == 'O' && resultData[1] == 'K') {
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }
                        //send file name
                        sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                        int sendResult = 1;
                        //dopoki nie koniec pliku to piszemy do kolejki
                        while (sendResult == 1) {
                            sendResult = getDataFromDifferentUser(newSocketId, resultData, true);
                            //jesli -1 to poleciał timeout
                            if (sendResult != -1) {
                                writemsg(numberOfQueueToWrite, resultData);
                            }
                        }

                    }
                }
                closeSocket(newSocketId);
            }

            //połączenie do innego peeru
            if (option == '4') {
                newSocketId = createSocket(port, clientAddress);
                int readMsg = readmsg(numberOfQueueToRead, buffer);
                if (readMsg != 0) {
                    break;
                }

                struct sockaddr_in6 client;
                inet_pton(AF_INET6, buffer, &(client.sin6_addr));
                client.sin6_port = htons(3001);
                client.sin6_family = AF_INET6;

                if (connectToDifferentSocket(newSocketId, client) == 0) {
                    char fileName[64];
                    int readMsg = readmsg(numberOfQueueToRead, fileName);
                    if (readMsg != 0) {
                        break;
                    }
                    //send file name
                    sendDataToDifferentUser(newSocketId, fileName, MSG_LENGTH - 1, true);
                    char resultData[1024];
                    int sendResult = 1;
                    FILE *file;
                    //TODO: nie jestem pewna czy tu powinnam tego szukać (do dopytania u Zu)
                    char folder[] = "../sharedFiles/";
                    //odczytujemy nazwę pliku
                    char name[40];
                    int count = 0;
                    char c;
                    while ((c = *fileName) != '\n'){
                        name[count] = c;
                        count++;
                    }
                    char finalName[count];
                    for(int i = 0; i< count; ++i){
                        finalName[0] = name [0];
                    }
                    strcat(finalName, folder);

                    file = fopen(folder, "w");
                    //dopoki nie koniec pliku to piszemy do kolejki
                    while (sendResult == 1) {
                        sendResult = getDataFromDifferentUser(newSocketId, resultData, true);
                        //jesli -1 to poleciał timeout
                        if (sendResult != -1) {
                            fprintf(file, "%s", resultData);
                        } else {
                            break;
                        }
                    }
                    //zamykamy połączenie z peerem
                    closeSocket(newSocketId);

                    //wysyłamy info o posiadaniu plików do trackera
                    newSocketId = createSocket(port, clientAddress);
                    //jeśli połaczenie do trackera się powiodło
                    if (connectToDifferentSocket(newSocketId, tracker) == 0) {
                        sendDataToDifferentUser(newSocketId, "1", 1, true);
                        getDataFromDifferentUser(newSocketId, resultData, true);
                        if (resultData[0] == 'O' && resultData[1] == 'K') {
                            sendDataToDifferentUser(newSocketId, fileName, MSG_LENGTH - 1, true);
                        }
                    }
                }
                //zamykamy ostatnio utworzony socket
                closeSocket(newSocketId);

            }

        }
    }
}

void *socketSupervisorModule(void *clientAddress) {
    pthread_t listensocket_thread_id;
    pthread_t connectsocket_thread_id;

    int mode = 0;
    printf("Please input application mode (0 for server, 1 for client): ");
    scanf("%d", &mode);

    //do nasłuchiwania połączeń
    if (mode == 0) {
        if (pthread_create(&listensocket_thread_id, NULL, listenSection, clientAddress)) {
            printf("Failed to create ListenSocket\n");
            exit(1);
        }
        pthread_join(listensocket_thread_id, NULL);
    }

    //do obsługi kolejki i nawiązywania połączeń z innymi peerami
    if (mode == 1) {
        if (pthread_create(&connectsocket_thread_id, NULL, queueSection, clientAddress)) {
            printf("Failed to create ConnectSocket\n");
            exit(1);
        }
        pthread_join(connectsocket_thread_id, NULL);
    }
}

int main() {
    for (int i = 0; i < MODULE_COUNT; i++) {
        if (pthread_mutex_init(writemsg_lock + i, NULL) != 0) {
            printf("\n mutex #%i init failed\n", i);
            return 1;
        }
    }
    printf("Please input tracker address: ");
    char tracker[40];
    scanf("%s", tracker);
    inet_pton(AF_INET6, tracker, &(trackerAddress));

    printf("Tracker address parsed successfully.\n");

    pthread_t supervisor_thread_id;
    struct sockaddr_in6 clientAddress;

    inet_pton(AF_INET6, "::1", &(clientAddress));

    char buffer[64];
    inet_ntop(AF_INET6, &clientAddress, buffer, sizeof(clientAddress));
    printf("IP parsed in main: %s\n", buffer);

//TEST//

    writemsg(1, "4");
    writemsg(1, "::1");
    writemsg(1, "test.txt");

//!TEST//

    if (pthread_create(&supervisor_thread_id, NULL, socketSupervisorModule, (void *) &clientAddress)) {
        printf("Failed to create ConnectSocket\n");
        exit(1);
    }

    pthread_join(supervisor_thread_id, NULL);

    return 0;
}
