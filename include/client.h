#ifndef TIN_CLIENT_H
#define TIN_CLIENT_H
#include <arpa/inet.h>
#include "queue.h"
#include "user.h"
#include "socket.h"
struct in6_addr trackerAddress;
void *listenSection(void *userAddr);
void *queueSection(void *clientAddr);
void *socketSupervisorModule(void *clientAddress);
void setTrackerAddress(char tracker[40]);
#endif
