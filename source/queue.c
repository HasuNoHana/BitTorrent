#include "../include/queue.h"

pthread_mutex_t writemsg_lock[MODULE_COUNT];
 
char msg_array[MODULE_COUNT][MSG_ARRAY_SIZE][MSG_LENGTH];
int msg_read[MODULE_COUNT] = {0, 0, 0, 0};
int msg_write[MODULE_COUNT] = {0, 0, 0, 0};
 
 
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

int prepareQueueMutexes()
{
    for(int i = 0; i < MODULE_COUNT; i++)
    {
        if (pthread_mutex_init(writemsg_lock+i, NULL) != 0)
        {
            printf("\n Mutex #%i init failed\n", i);
            return 1;
        }
    }
    return 0;
}