#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "include/queue.h"
#include "include/client.h"
#include "include/interface.h"

int main() {
    prepareQueueMutexes();

    printf("Please input tracker address: ");
    char tracker[40];
    scanf("%s", tracker);
    setTrackerAddress(tracker);

    printf("Tracker address parsed successfully.\n");

    pthread_t supervisor_thread_id;
    pthread_t io_thread_id;
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
//    writemsg(1, "plik1");
//    writemsg(1, "2");
//    writemsg(1, "plik2");
//    writemsg(1, "4");
//    writemsg(1, "::1");
//    writemsg(1, "sharedFiles/torrent.txt");
//    writemsg(1, "0");

//!TEST//

    if (pthread_create(&supervisor_thread_id, NULL, socketSupervisorModule, (void *) &clientAddress)) {
        perror("Failed to create SocketSupervisor Module thread\n");
        exit(1);
    }

    if (pthread_create(&io_thread_id, NULL, ioModule, (void *) &clientAddress)) {
        perror("Failed to create I/O Module thread\n");
        exit(1);
    }

    pthread_join(io_thread_id, NULL);
    destroyQueueMutexes();

    return 0;
}
