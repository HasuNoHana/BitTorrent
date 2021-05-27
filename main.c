#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/socket.h"
#include "include/user.h"
#include <unistd.h>

#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 4

pthread_mutex_t writemsg_lock[MODULE_COUNT];

char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
int msg_read[MODULE_COUNT] = {0, 0, 0, 0};
int msg_write[MODULE_COUNT] = {0, 0, 0, 0};


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

int socketModule(struct in6_addr trackerAddress, struct in6_addr clientAddress) {
    bool run = true;
    int numberOfQueueToRead = 1;
    int numberOfQueueToWrite = 2;
    int newSocketId;
    int port = 3030;

    struct sockaddr_in6 tracker;
    tracker.sin6_addr = trackerAddress;
    tracker.sin6_port = 8080;
    tracker.sin6_family = AF_INET6;

    pid_t pid;
    int i;

    for (i = 0; i < 2; i++) {
        //tworzymy dziecko
        pid = fork();

        //dziecko ma pid 0, więc opuszcza pętlę
        if (pid == 0) {
            break;
        }
            //jeśli pid mniejszy od zera coś poszło nie tak, wyrzucamy błąd
        else if (pid < 0) {
            exit(1);
        }
    }

    if(pid == 0){
        //do nasłuchiwania połączeń
        if(i == 0){
            while(run){
                int socketToListen = createSocket(3001, clientAddress);
                listenToConnect(socketToListen);
                //akceptuje
                //pobiera dane jaki będzie to plik
                //pobiera plik z jakiejś funkcji (?)
                //wysyła plik i wraca do słuchania
            }

        }

        //do obsługi kolejki
        if(i == 1){
            while (run) {
                char *buffer;
                if (readmsg(numberOfQueueToRead, buffer) == 0) {
                    char option = *buffer;
                    switch (option) {
                        //koniec słuchania, zamykamy wszystko
                        case 0:
                            run = false;
                            break;
                            //dodanie pliku (info do trackera)
                        case 1:
                            buffer++;
                            newSocketId = createSocket(port, clientAddress);
                            //jeśli połaczenie do trackera się powiodło
                            if (connectToDifferentSocket(newSocketId, tracker) == 0) {
                                sendDataToDifferentUser(newSocketId, "1", 1, true);
                                char *data = getDataFromDifferentUser(newSocketId, true);
                                if (data != NULL) {
                                    sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                                }
                            }
                            closeSocket(newSocketId);
                            break;
                            //usuniecie pliku (info do trackera)
                        case 2:
                            buffer++;
                            newSocketId = createSocket(port, clientAddress);
                            //jeśli połaczenie do trackera się powiodło
                            if (connectToDifferentSocket(newSocketId, tracker) == 0) {
                                sendDataToDifferentUser(newSocketId, "2", 1, true);
                                char *data = getDataFromDifferentUser(newSocketId, true);
                                if (data != NULL) {
                                    sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                                }
                            }
                            closeSocket(newSocketId);
                            break;
                            //pobranie listy peerów z określonym plikiem
                        case 3:
                            buffer++;
                            newSocketId = createSocket(port, clientAddress);
                            //jeśli połaczenie do trackera się powiodło
                            if (connectToDifferentSocket(newSocketId, tracker) == 0) {
                                sendDataToDifferentUser(newSocketId, "3", 1, true);
                                char *data = getDataFromDifferentUser(newSocketId, true);
                                if (data != NULL) {
                                    if (sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true) != -1) {
                                        char *listData = getDataFromDifferentUser(newSocketId, true);
                                        writemsg(numberOfQueueToWrite, listData);
                                    }
                                }
                            }
                            closeSocket(newSocketId);
                            break;
                        //pobierz dane do konkretnego peeru
                        case 4:
                            //łączy się ze wskazanym peerem na adresie 3000
                            //pobiera dane
                            //wysyła do trackera info, że ma taki plik
                            break;

                        default:
                            break;
                    }
                }

            }
        }
    }


    return 0;
}

int main() {
    for (int i = 0; i < MODULE_COUNT; i++) {
        if (pthread_mutex_init(writemsg_lock + i, NULL) != 0) {
            printf("\n mutex #%i init failed\n", i);
            return 1;
        }
    }

    return 0;
}

