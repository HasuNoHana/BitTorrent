#include "../include/client.h"
#include "../include/queue.h"
#include <string.h>

struct in6_addr trackerAddress;

void *listenSection(void *userAddr) {
    struct in6_addr userAddress = *(struct in6_addr *) userAddr;

    char buffer[64];
    inet_ntop(AF_INET6, &userAddress.s6_addr, buffer, sizeof(userAddress));
    printf("IP passed to ListenSocket: %s\n", buffer);

    while (1) {
        char fileName[64];

        int socketToListen = createSocket(3001, userAddress);
        listenToConnect(socketToListen);

        struct sockaddr_in6 client;
        int acceptSocket = acceptConnection(socketToListen, client);
        if ( acceptSocket == -1) {
            break;
        } else {
            char buf6[INET6_ADDRSTRLEN];
            struct in6_addr check_address = client.sin6_addr;
            inet_ntop(AF_INET6, &check_address, buf6, sizeof(buf6));

            printf("Accepted connection from %s\n", buf6);

            getDataFromDifferentUser(acceptSocket, fileName, true);

            FILE *file = fopen(fileName, "r");
            if (file == NULL) {
                printf("Open file %s  to read failed!", fileName);
                break;
            }

            char sendData[1024];
            while (fgets(sendData, 1024, file) != NULL) {
                sendDataToDifferentUser(acceptSocket, sendData, 1024, true);
                bzero(sendData, 1024);
            }
            fclose(file);
            printf("File %s sent!\n", fileName);
            closeSocket(acceptSocket);
            closeSocket(socketToListen);
        }
    }
}

void *queueSection(void *clientAddr) {

    struct in6_addr clientAddress = *(struct in6_addr *) clientAddr;

    int numberOfQueueToRead = socketQueue;
    int numberOfQueueToWrite = ioQueue;
    int newSocketId;
    int port = 3030;

    struct sockaddr_in6 tracker;
    tracker.sin6_addr = trackerAddress;
    tracker.sin6_port = htons(8080);
    tracker.sin6_family = AF_INET6;


    while (1) {
        char buffer[64];
        if (readmsg(numberOfQueueToRead, buffer) == 0){
            char option = *buffer;

            //koniec s??uchania, zamykamy wszystko
            if (option == '0') {
                break;
            }

            //przypadki gdy po????czenie "idzie" do trackera
            if (option == '1' || option == '2' || option == '3') {

                //tworzymy nowy socket i bindujemy mu nasz adres
                newSocketId = createSocket(port, clientAddress);
                connectToDifferentSocket(newSocketId, tracker);

                //wysylamy informacj?? do trackera co chcemy zrobi??
                sendDataToDifferentUser(newSocketId, &option, 1, true);
                char resultData[1024];

                //tracker nam odpowiada
                getDataFromDifferentUser(newSocketId, resultData, true);

                if (option == '1') {
                    if (resultData[0] != '0') {
                        //rozmiar pliku torrent
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }

                        FILE *file;
                        //liczymy rozmiar pliku

                        int size = atoi(buffer);
                        char fileName[64];
                        readMsg = readmsg(numberOfQueueToRead, fileName);
                        if (readMsg != 0) {
                            break;
                        }

                        file = fopen(fileName, "r");
                        if (file == NULL) {
                            printf("Open file %s failed!", fileName);
                            break;
                        }

                        sendFileToTracker(file, newSocketId);

                        //char sendData[1024];
                        //while(fgets(sendData, 1024, file) != NULL) {
                        //   sendDataToDifferentUser(newSocketId, sendData, size, true);
                        //   bzero(sendData, 1024);
                        //}

                        fclose(file);
                        printf("Torrent file sent!\n");
                    }
                    closeSocket(newSocketId);
                } else if (option == '3') {
                    puts("Delete file in tracker.\n");
                    if (resultData[0] != '0') {
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }
                        sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);
                        getDataFromDifferentUser(newSocketId, buffer, true);
                        if(buffer[0] != '0'){
                            printf("%s", buffer);
                        }
                    }
                    closeSocket(newSocketId);

                } else {
                    puts("Get file list from tracker.\n");
                    if (resultData[0] != '0') {
                        int readMsg = readmsg(numberOfQueueToRead, buffer);
                        if (readMsg != 0) {
                            break;
                        }
                        //wysy??amy nazw?? pliku
                        sendDataToDifferentUser(newSocketId, buffer, MSG_LENGTH - 1, true);

                        //przygotowujemy si?? do czytania odpowiedzi
                        int sendResult = 0;
                        char readData[1024];
                        for(int i = 0; i <1024 ; ++i){
                            readData[i]= 0;
                        }
                        //dopoki nie koniec pliku to piszemy do kolejki
                        do{
                            sendResult = getDataFromDifferentUser(newSocketId, readData, true);
                            if(readData[0] != '-' ){
                                puts("Data receiving...\n");

                                int zeroCounter = 0;
                                int msgSize = 0;

                                for(int i = 0; i <1024 ; i++){
                                    printf("%d ", readData[i], readData[i]);
                                    ++msgSize;
                                    if(readData[i] == 0)
                                    {
                                        zeroCounter++;
                                        readData[i] = 63;
                                    }
                                    else
                                        zeroCounter = 0;

                                    if(zeroCounter == 3)
                                    {
                                        readData[i-2] = 0;
                                        msgSize -= 3;
                                        break;
                                    }
                                }

                                char dataToSend[1024];
                                strncpy(dataToSend, readData, msgSize);
                                if(msgSize != 1024) {
                                    dataToSend[msgSize] = '\0';
                                }

                                for(int i = 0; i <1024 ; i++){
                                    readData[i] = 0;
                                }

                                parseTrackerDataAndPostToQueue(numberOfQueueToWrite, msgSize, dataToSend);

                                //writemsg(numberOfQueueToWrite, resultData);
                                printf("\n\n->%s\n", dataToSend);
                            }
                            else{
                                sendResult = -1;
                                printf("File %s not found in tracker\n", buffer);
                            }
                        }
                        while (sendResult != -1);
                    }
                    closeSocket(newSocketId);
                }

            }

            //po????czenie do innego peeru
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

                if (connectToDifferentSocket(newSocketId, client) == 1)
                {
                    writemsg(numberOfQueueToWrite, "failure");
                }

                else if (connectToDifferentSocket(newSocketId, client) == 0) {
                    writemsg(numberOfQueueToWrite, "success");

                    char fileName[64];
                    int readMsg = readmsg(numberOfQueueToRead, fileName);
                    if (readMsg != 0) {
                        break;
                    }
                    printf("%s\n", fileName);
                    //send file name
                    sendDataToDifferentUser(newSocketId, fileName, MSG_LENGTH, true);

                    FILE *file;

                    //URUCHAMIAJ??C NA JEDNEJ MASZYNIE NALEZY ZMIENIC ZEBY NIE CZYTALO I PISALO Z JEDNEGO PLIKU BO NICZEGO NIE PRZECZYTA
                    file = fopen(fileName, "w");
                    if (file == NULL) {
                        printf("Open file %s to write failed!", fileName);
                        break;
                    }

                    char resultData[1024];
                    int sendResult = 1;
                    bool received = false;

                    //dopoki nie koniec pliku to piszemy do pliku
                    while (sendResult == 1) {
                        sendResult = getDataFromDifferentUser(newSocketId, resultData, true);
                        //jesli -1 to polecia?? timeout
                        if (sendResult != -1) {
                            fprintf(file, "%s", resultData);
                            received = true;
                        } else {
                            break;
                        }
                    }
                    fclose(file);
                    //zamykamy po????czenie z peerem
                    closeSocket(newSocketId);
                    if(received){
                        //wysy??amy info o posiadaniu plik??w do trackera
                        newSocketId = createSocket(port, clientAddress);
                        //je??li po??aczenie do trackera si?? powiod??o
                        if (connectToDifferentSocket(newSocketId, tracker) == 0) {
                            sendDataToDifferentUser(newSocketId, "4", 1, true);
                            getDataFromDifferentUser(newSocketId, resultData, true);
                            if (resultData[0] != '0') {
                                sendDataToDifferentUser(newSocketId, fileName, MSG_LENGTH - 1, true);
                                getDataFromDifferentUser(newSocketId, resultData, true);
                            }
                        }
                        closeSocket(newSocketId);
                    }
                }
                //zamykamy ostatnio utworzony socket


            }

        }
        else{
        	sleep(1);	//je??li modu?? jest bezczynny, usypiamy go do wyeliminowania ci????kiego active pollingu
        }
    }

}

void *socketSupervisorModule(void *clientAddress) {
    pthread_t listensocket_thread_id;
    pthread_t connectsocket_thread_id;

    /*

    int mode = 0;
    printf("Please input application mode (0 for server, 1 for client): ");
    scanf("%d", &mode);


    //do nas??uchiwania po????cze??
    if (mode == 0) {
        if (pthread_create(&listensocket_thread_id, NULL, listenSection, clientAddress)) {
            printf("Failed to create ListenSocket\n");
            exit(1);
        }
        pthread_join(listensocket_thread_id, NULL);
    }

    //do obs??ugi kolejki i nawi??zywania po????cze?? z innymi peerami
    if (mode == 1) {
        if (pthread_create(&connectsocket_thread_id, NULL, queueSection, clientAddress)) {
            printf("Failed to create ConnectSocket\n");
            exit(1);
        }
        pthread_join(connectsocket_thread_id, NULL);
    }

    */

    if (pthread_create(&listensocket_thread_id, NULL, listenSection, clientAddress)) {
            printf("Failed to create ListenSocket\n");
            exit(1);
        }
    if (pthread_create(&connectsocket_thread_id, NULL, queueSection, clientAddress)) {
            printf("Failed to create ConnectSocket\n");
            exit(1);
        }
    pthread_join(connectsocket_thread_id, NULL);
    pthread_join(listensocket_thread_id, NULL);
}
void setTrackerAddress(char tracker[40]){
    inet_pton(AF_INET6, tracker, &(trackerAddress));
}
