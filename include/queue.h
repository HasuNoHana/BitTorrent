#ifndef TIN_QUEUE_H
#define TIN_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 3

#define metadataQueue 0
#define socketQueue 1
#define ioQueue 2

extern pthread_mutex_t writemsg_lock[MODULE_COUNT];

extern char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
extern int msg_read[MODULE_COUNT];
extern int msg_write[MODULE_COUNT];

int writemsg(int ID, char *message);
int readmsg(int ID, char *buffer);

void sock_DownloadFromPeer(char ipAddress[], char fileName[]);
void sock_PostFileToTracker(char fileName[], int fileSize);
void sock_DeleteFileFromTracker(char fileName[]);
void sock_RequestFileList(char fileName[]);

void parseTrackerDataAndPostToQueue(int QueueID, int dataSize, char data[]);
void downloadFile(char fileName[], char clientIP[]);

int prepareQueueMutexes();
int destroyQueueMutexes();

#endif