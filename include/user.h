#ifndef TIN_USER_H
#define TIN_USER_H

#include <stdbool.h>
#include <stdio.h>
#include "socket.h"
//funkcja zwraca 0 jesli zakończyła się sukcesem i 1 gdy był błąd
int sendDataToDifferentUser(int socketId, char* data, int dataSize, bool trackerOperation);
//funkcja pobierająca dane od innego użytkownika
int getDataFromDifferentUser(int userSocketId, char resultData[1024], bool trackerOperation);
#endif
