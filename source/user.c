#include "../include/user.h"

int sendDataToDifferentUser(int userSocketId, char *data, int dataSize, bool trackerOperation) {
    int sendDataNumber = 0;
    int send;

    while (sendDataNumber < dataSize) {
        send = sendData(userSocketId, data, dataSize - sendDataNumber);
        //jesli coś się wysłało, to przesuwamy wskaźnik
        if (send >= 0) {
            sendDataNumber += send;
            for (int i = 0; i < send; ++i) {
                data++;
            }
        }
    }
    // wszystko się udało, zwracamy 0
    return 0;
}

int getDataFromDifferentUser(int userSocketId, char resultData[1024], bool trackerOperation) {

    int dataNumber = 0;
    int result = 1;
    while (dataNumber < 1024) {
        char tmpBuffer[1024];
        int numberOfData = receiveData(userSocketId, 1024 - dataNumber, tmpBuffer);

        if (numberOfData == 0) {
            //ostatnia "paczka" zamykamy połączenie
            closeConnection(userSocketId);
            result = -1;
            break;
        } else {
            for (int i = 0; i < numberOfData; ++i) {
                resultData[dataNumber] = tmpBuffer[i];
                ++dataNumber;
            }
            if(tmpBuffer[numberOfData] == -1 || tmpBuffer[numberOfData] == 0){
                puts("End of data.\n");
                result = -1;
                break;
            }
        }
    }
    return result;

}

