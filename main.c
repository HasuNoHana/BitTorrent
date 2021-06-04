#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define WHITEBACK "\033[7;37m"
#define RESET "\033[0m"
#define MENU_TAB_COUNT 3
#define MENU_OPTION_COUNT 2
 
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
    if(markerpos == 0)//Delete File
    {

    } else if (markerpos == 1)//Share File
    {

    } else if (markerpos == 2){//Download File

    } else{
        printf("There is no Menu Option!");
        exit(1);
    }

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
//    printf("%s", "Press 'q' to exit");
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
}