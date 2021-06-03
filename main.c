#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "include/queue.h"
#include "include/client.h"

#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 4


int main() {
    prepareQueueMutexes();

    printf("Please input tracker address: ");
    char tracker[40];
    scanf("%s", tracker);
    setTrackerAddress(tracker);

    printf("Tracker address parsed successfully.\n");

    pthread_t supervisor_thread_id;
    struct sockaddr_in6 clientAddress;

    inet_pton(AF_INET6, "::1", &(clientAddress));

    char buffer[64];
    inet_ntop(AF_INET6, &clientAddress, buffer, sizeof(clientAddress));
    printf("IP parsed in main: %s\n", buffer);

//TEST//
//    writemsg(1, "1");
//    writemsg(1, "163");
//    writemsg(1, "sharedFiles/torrent.txt");
//    writemsg(1, "3");
//    writemsg(1, "plik3");
//    writemsg(1, "3");
//    writemsg(1, "plik2");
//    writemsg(1, "2");
//    writemsg(1, "plik4");
//    writemsg(1, "2");
//    writemsg(1, "plik2");
    writemsg(1, "4");
    writemsg(1, "::1");
    writemsg(1, "sharedFiles/torrent.txt");
    writemsg(1, "0");

//!TEST//

    if (pthread_create(&supervisor_thread_id, NULL, socketSupervisorModule, (void *) &clientAddress)) {
        printf("Failed to create ConnectSocket\n");
        exit(1);
    }

    pthread_join(supervisor_thread_id, NULL);
    destroyQueueMutexes();

    return 0;
}
