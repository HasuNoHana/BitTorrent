#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "queue.h"
#include "client.h"
#include "metadata.h"

#define WHITEBACK "\033[7;37m"
#define RESET "\033[0m"
#define MENU_TAB_COUNT 3
#define MENU_OPTION_COUNT 2

/* Initialize new terminal i/o settings */
void initTermios(int echo);

/* Switch back to old terminal i/o settings */
void resetTermios(void);

/* Switch to new terminal i/o settings */
/* Only use after initTermios has been called */
void overrideTermios(void);

extern char clientIP[40];

extern char menu_tabs[MENU_TAB_COUNT][15];
extern char menu_options[MENU_TAB_COUNT][MENU_OPTION_COUNT][15];

extern int menu_markerpos;
extern int options_markerpos[MENU_TAB_COUNT];

void printUI();
void choseOption(int markerpos);
void menuInput(char control);

void* ioModule();
