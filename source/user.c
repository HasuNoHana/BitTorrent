#include "../include/user.h"

int sendDataToDifferentUser(int userSocketId, char *data, int dataSize, bool trackerOperation) {
    int sendDataNumber = 0;
    int send = 0;

    int socketReady;
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(userSocketId, &set);
    if (trackerOperation) {
        //timeout dla trackera = 45 s
        timeout.tv_sec = 45;
        timeout.tv_usec = 0;
    } else {
        //timeout dla użytkownikow = 30 s
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
    }

    while (sendDataNumber < dataSize) {
        socketReady = select(userSocketId + 1, &set, NULL, NULL, &timeout);
        if (socketReady == 1) {
            send = sendData(userSocketId, data, dataSize - sendDataNumber);
            //jesli coś się wysłało, to przesuwamy wskaźnik
            if (send >= 0) {
                sendDataNumber += send;
                for (int i = 0; i < send; ++i) {
                    data++;
                }
                send = 0;
            }
        } else {
            //jeśli poleciał timeout zamykamy połączenie i zwracamy -1
            closeConnection(userSocketId);
            return -1;
        }
    }
    // wszystko się udało, zwracamy 0
    return 0;
}

int getDataFromDifferentUser(int userSocketId, char resultData[1024], bool trackerOperation) {
    int socketReady;
    fd_set set;
    struct timeval timeout;
    int sendDataNumber = 0;

    FD_ZERO(&set);
    FD_SET(userSocketId, &set);
    if (trackerOperation) {
        //timeout dla trackera = 45 s
        timeout.tv_sec = 45;
        timeout.tv_usec = 0;
    } else {
        //timeout dla użytkownikow = 30 s
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
    }

    while (sendDataNumber < 1024) {
        socketReady = select(userSocketId + 1, &set, NULL, NULL, &timeout);
        if (socketReady == 1) {
            char tmpBuffer[1024];
            int numberOfData = receiveData(userSocketId, 1024 - sendDataNumber, tmpBuffer);
            if (numberOfData == 0) {
                //ostatnia "paczka" zamykamy połączenie
                closeConnection(userSocketId);
                return 0;
            } else {
                for (int i = 0; i < numberOfData; ++i) {
                    resultData[sendDataNumber] = tmpBuffer[i];
                    ++sendDataNumber;
                }
            }
        } else {
            //jeśli poleciał timeout zamykamy połączenie i zwracamy -1
            closeConnection(userSocketId);
            return -1;
        }
    }
    return 1;

}

