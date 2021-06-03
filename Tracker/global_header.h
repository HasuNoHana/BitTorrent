#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

#define FILEN_LENGTH 40     //max length of a file
#define IP_MAX 39           //max length of the IP Adress - 32 hex digits + 7 x ':'
#define PORT_MAX 5          //max length of port number
#define PIECE_LENGTH 32     //max length of files' piece hash
#define DATASIZE 1024       //size of data sent in send() function, or received in recv() function

/*
 * Structure that represents one piece of file.
 */
typedef struct PieceS{
    char* pieceHash;
    struct PieceS* next;
} Piece;

/*
 * Structure that represents one file's owner(seed).
 * IsOriginal - variable that contains information whether this file owner has original file. 
 */
typedef struct SeedS{
    int isOriginal;
    char* seedIP;
    struct SeedS* next;
} Seed;

/*
 * Structure that represents one file.
 * Contains list of file pieces and list of file owners.
 */
typedef struct FileS{
    char* filename;
    Piece* piece_head;
    Seed* seed_head;
    struct FileS* next;
} File_S;

/* 
 * Structure that contains arguments of function called by pthread_create();
 */
typedef struct thread_args{
    int client_socket;
    char* clientIP;
} ThreadArgs;

extern File_S *head;            //Pointer to the head of file list.
extern int socketDescriptor;    //Tracker socket descriptor.
extern pthread_mutex_t lock;    //Mutex lock.

// main utilities:
/*
 *  Dereferences Tracker IP from main function arguments.
 */
char* getIP(char* str);
/*
 *  Dereferences Tracker Port from main function arguments.
 */
char* getPort(char* str);


//file_handler:
/*
 * Returns pointer to a file with given filepath.
 * mode = 1 <=> 'w'
 * mode = 2 <=> 'r'
 */
FILE* getFilePointer(int mode, char* filepath);
/*
 * Reads seeders from initial file, after Tracker's startup.
 */
void loadSeedersFromInitialFile(char* filepath);
/*
 * Returns name of a file, deferenced from given file line.
 */
char* getFilename(char* line);
/*
 * Returns IP of original file owner, deferenced from given file line.
 */
char* getOwnerIP(char* line);
/*
 * Returns IP of file owner, deferenced from given file line.
 */
char* getSeedIP(char* line);


//socket:
/*
 * Creates socket and listens for client.
*/
void createSocket(const char* mytrackerIP, const char* mytrackerPort);
/*
 * Serves requests from client.
 */
void *serveRequest(void *client_socket);
/*
 * Returns filepath to file received from client.
 */
char* receiveFile(int client_socket, char* clientIP);
/*
 *  Sends file to client.
 */
void sendFile(FILE *fp, int client_socket);


//client_service:
/*
 * Gets torrent file from client, than reeds information from torrent file
 * and adds them to File List.
 */
void addFile(int client_socket, char* clientIP);
/*
 * Sends pieces and seeds list of the file to client.
 * filename - name of the file.
 */
void download(int client_socket, char* clientIP, char* filename);
/*
 * Removes client from file owners list. 
 */
void removeSeeder(int client_socket, char* clientIP, char* filename);
/*
 * Adds client to file owners list. 
 */
void updateSeedersList(int client_socket, char* clientIP, char* filename);
/*
 * Returns string with trimmed whitespaces. 
 */
char *trimwhitespace(char *str);


//filelist_handler:
/*
 * Inserts piece of file to file list. With piece hash entangled in file line. 
 */
void insertPiece(char* filename, char* line, char* clientIP, int isOriginal, int readLine);
/*
 * Checks whether file exists in File List. 
 */
int checkFile(char* filename);
/*
 * Loops over list of Files and gives information about them to output.
 */
void loopSeeders();
/*
 * Carries out commands from functions from the customer service section.
 */
int accessFilesList(int mode, char* filename, char* filepath, char* clientIP);


int getDataFromDifferentUser(int userSocketId, char resultData[1024], int trackerOperation);
int receiveData(int socketDescriptor, int dataSize, char data[1024]);