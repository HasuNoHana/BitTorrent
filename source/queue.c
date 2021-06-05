#include "../include/queue.h"
#include <string.h>
#include <pthread.h>

pthread_mutex_t writemsg_lock[MODULE_COUNT];
 
char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
int msg_read[MODULE_COUNT] = {0, 0, 0};
int msg_write[MODULE_COUNT] = {0, 0, 0};
 
 
int writemsg(int ID, char *message)
{
    pthread_mutex_lock(writemsg_lock+ID);
 
    if((msg_write[ID] == msg_read[ID] - 1 || msg_write[ID] == msg_read[ID] + MSG_ARRAY_SIZE - 1 % MSG_ARRAY_SIZE))
        return -1;
 
    strcpy(msg_array[ID][msg_write[ID]], message);
 
    msg_write[ID] = (msg_write[ID] + 1) % MSG_ARRAY_SIZE;
 
    pthread_mutex_unlock(writemsg_lock+ID);
 
    return 0;
}
 
int readmsg(int ID, char *buffer)
{
    if(msg_read[ID] == msg_write[ID])
        return -1;
 
    int msg_read_old = msg_read[ID];
    msg_read[ID] = (msg_read[ID] + 1) % MSG_ARRAY_SIZE;
 
    strcpy(buffer, msg_array[ID][msg_read_old]);
 
    return 0;
}

void sock_DownloadFromPeer(char ipAddress[], char fileName[])
{
    writemsg(socketQueue, "4");
    writemsg(socketQueue, ipAddress);
    writemsg(socketQueue, fileName);
    writemsg(socketQueue, "0");

    return;
}

void sock_PostFileToTracker(char fileName[], int fileSize)
{
    writemsg(socketQueue, "1");
    char buffer[12];
    sprintf(buffer, "%d", fileSize);
    writemsg(socketQueue, buffer);
    writemsg(socketQueue, fileName);

    return;
}

void sock_DeleteFileFromTracker(char fileName[])
{
    writemsg(socketQueue, "3");
    writemsg(socketQueue, fileName);

    return;
}

void sock_RequestFileList(char fileName[])
{
    writemsg(socketQueue, "2");
    writemsg(socketQueue, fileName);

    return;
}

void parseTrackerDataAndPostToQueue(int QueueID, int dataSize, char data[])
{
    puts("\nParsing started...\n");

    char ipFlag = 0;
    char compareBuf[] = "seeds:";
    char buffer[128];

    int counter = 0;
    int last = 0;

    printf("ip addresses seeding the file:\n\n");

    while(data[counter] != 0 && counter < dataSize)
    {
        memset(buffer, 0, sizeof(buffer));
        if(data[counter] == 10 || counter == dataSize - 1) //getline lub EOF
        {
            strncpy(buffer, data+last, counter-last);
            last = counter+1;   //pomijamy newline

            if(ipFlag == 0)
            {
                if(strcmp(buffer, compareBuf) == 0) //znaleziono linię seeds:, teraz można czytać linie IP
                {
                    ipFlag = 1;
                }
            }

            else
            {
                printf("%s\n", buffer);
                writemsg(QueueID, buffer);
            }            
        }
        counter++;
    }
    writemsg(QueueID, "done");
    return;
}

void downloadFile(char fileName[], char clientIP[])
{
    char buffer[40];
    char ipArray[16][40];
    int ipCount = 0;
    sock_RequestFileList(fileName);
    sleep(3);     //funkcja czeka na przetworzenie danych przez moduł socketowy

    for(int i = 0; i < 16; i++) //zapisz wszystkie odczytane adresy ip do tabeli
    {
        readmsg(ioQueue, buffer);

        if (strcmp(buffer, "done") == 0) {
            break;
        } else {
            if(strcmp(buffer, clientIP) != 0)
            {
                strcpy(ipArray[i], buffer);
                ++ipCount;
            }
        }
    }

    if(ipCount == 0)
    {
        puts("No vaild IP addresses were passed, aborting file download.");
        return;
    }

    while (strcmp(buffer, "done") != 0) //wyczyść kolejkę z nadmiarowych adresów ip
    {
        readmsg(ioQueue, buffer);
    }

    memset(buffer, 0, sizeof(buffer));

    while (strcmp(buffer, "success") != 0 && ipCount > 0)
    {
        sock_DownloadFromPeer(ipArray[ipCount-1], fileName);
        readmsg(socketQueue, buffer);
        --ipCount;
    }

    if(strcmp(buffer, "success") != 0)
    {
        printf("No seeds have responded. File could not be downloaded.\n");
    }

    return;
}

int prepareQueueMutexes()
{
    for(int i = 0; i < MODULE_COUNT; i++)
    {
        if (pthread_mutex_init(writemsg_lock+i, NULL) != 0)
        {
            printf("\n Mutex #%i init failed", i);
            perror("");
            return 1;
        }
    }
    return 0;
}

int destroyQueueMutexes()
{
    for(int i = 0; i < MODULE_COUNT; i++)
    {
        if (pthread_mutex_destroy(writemsg_lock+i) != 0)
        {
            printf("\n Mutex #%i could not be destroyed", i);
            perror("");
            return 1;
        }
    }
    return 0;
}