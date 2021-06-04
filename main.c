#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "include/queue.h"
#include "include/client.h"
//#include "include/queue.h"

#define WHITEBACK "\033[7;37m"
#define RESET "\033[0m"
#define MENU_TAB_COUNT 3
#define MENU_OPTION_COUNT 2
#define MSG_LENGTH 64
#define MSG_ARRAY_SIZE 32
#define MODULE_COUNT 4

/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    //tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

/* Switch back to old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

/* Switch to new terminal i/o settings */
/* Only use after initTermios has been called */
void overrideTermios(void) {
    tcsetattr(0, TCSANOW, &new1);
}

char menu_tabs[MENU_TAB_COUNT][15] = {"Delete File", "Share File", "Download File"};
char menu_options[MENU_TAB_COUNT][MENU_OPTION_COUNT][15] = {{}, {}, {}};

int menu_markerpos = 0;
int options_markerpos[MENU_TAB_COUNT] = {0};

//void initializeMenuOptions(){
//    menu_options[MENU_TAB_COUNT] =
//            malloc(sizeof(Node) * MENU_TAB_COUNT);
//    for( int i=0; i<MENU_TAB_COUNT; i++){
//        menu_options[i] = mall
//    }
//}

void printUI()
{
    system("clear");
    //print title
    printf("BitTorrent client v0.1\n");
    printf("%s", "Press 'q' to exit\n");
    //print menu tabs
    for(int i = 0; i < MENU_TAB_COUNT; i++)
    {
        if(i == menu_markerpos)
        {
            printf(WHITEBACK);
            printf("%s", menu_tabs[i]);
            printf(RESET);
        }
        else
            printf("%s", menu_tabs[i]);
        printf("   ");
    }
    printf("\n\n");

    //print tabs' content
    for(int i = 0; i < 2; i++)
    {
        if(i == options_markerpos[menu_markerpos])
        {
            printf(WHITEBACK);
            printf("%s", menu_options[menu_markerpos][i]);
            printf(RESET);
        }
        else
            printf("%s", menu_options[menu_markerpos][i]);
        printf("\n");
    }
    printf("\n");
}

void choseOption(int markerpos) {//TODO implement
    resetTermios();
    if(markerpos == 0)//Delete File
    {

    } else if (markerpos == 1)//Share File
    {
        char filename[40];
        printf("What is the path to file that you wolud like to share?\n");
        scanf ("%40s",filename);
        int filesize = 2; //TODO call size counting function
//        sock_PostFileToTracker(filename, filesize);
        //TODO czy coś jeszcze trzeba tu zrobić?
    } else if (markerpos == 2){//Download File

    } else{
        printf("There is no Menu Option!");
        exit(1);
    }
    overrideTermios();

}

void menuInput(char control)
{
    switch(control)
    {
        case 'w':
            if(options_markerpos[menu_markerpos] == 0)
                options_markerpos[menu_markerpos] = MENU_OPTION_COUNT - 1;
            else
                --options_markerpos[menu_markerpos];
            break;

        case 's':
            if(options_markerpos[menu_markerpos] == MENU_OPTION_COUNT - 1)
                options_markerpos[menu_markerpos] = 0;
            else
                ++options_markerpos[menu_markerpos];
            break;

        case 'a':
            if(menu_markerpos == 0)
                menu_markerpos = MENU_TAB_COUNT - 1;
            else
                --menu_markerpos;
            break;

        case 'd':
            if(menu_markerpos == MENU_TAB_COUNT - 1)
                menu_markerpos = 0;
            else
                ++menu_markerpos;
            break;
        case '\n':
            printf("enter");
            printf("%d", menu_markerpos);
            printf(menu_tabs[menu_markerpos]);
            choseOption(menu_markerpos);
            break;
    }
}

int main(void)
{

    char c;
//    initializeMenuOptions();
    initTermios(0);
    overrideTermios();

    while (1)
    {
        printUI();
        read(0, &c, 1);
        if(c == 'q')
            break;
        menuInput(c);
    }

    resetTermios();
    return 0;

//    prepareQueueMutexes();
//
//    printf("Please input tracker address: ");
//    char tracker[40];
//    scanf("%s", tracker);
//    setTrackerAddress(tracker);
//
//    printf("Tracker address parsed successfully.\n");
//
//    pthread_t supervisor_thread_id;
//    struct sockaddr_in6 clientAddress;
//
//    inet_pton(AF_INET6, "::1", &(clientAddress));
//
//    char buffer[64];
//    inet_ntop(AF_INET6, &clientAddress, buffer, sizeof(clientAddress));
//    printf("IP parsed in main: %s\n", buffer);
//
////TEST//
////    writemsg(1, "1");
////    writemsg(1, "163");
////    writemsg(1, "sharedFiles/torrent.txt");
////    writemsg(1, "3");
////    writemsg(1, "plik3");
////    writemsg(1, "3");
////    writemsg(1, "plik2");
////    writemsg(1, "2");
////    writemsg(1, "plik4");
////    writemsg(1, "2");
////    writemsg(1, "plik2");
//    writemsg(1, "4");
//    writemsg(1, "::1");
//    writemsg(1, "sharedFiles/torrent.txt");
//    writemsg(1, "0");
//
////!TEST//
//
//    if (pthread_create(&supervisor_thread_id, NULL, socketSupervisorModule, (void *) &clientAddress)) {
//        printf("Failed to create ConnectSocket\n");
//        exit(1);
//    }
//
//    pthread_join(supervisor_thread_id, NULL);
//    destroyQueueMutexes();
//
//    return 0;
}
