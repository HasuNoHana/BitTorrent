//
// Created by zuznanna on 25.05.2021.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>


char* SHARED_FILE_EXTENSION = ".txt";
int SHARED_FILE_EXTENSION_LENGTH = 4;

/*
 * How bencoded torrent file looks like(without whitespaces):
 *
    d8:announce<url length>:<url>
    4:info d
    6:length i<file size>e
    4:name <file name length>:<file name>
    12:piece length i<piece length>e
    6:pieces  l<HashPiece1><...><HashPieceN>e
    e
    e
 */

char* getPathOfSharedFile(char *sharedFileName, int sharedFileNameSize){
    //size = folderName size + sharedFileName size + extension size + 1
    int s = 15+sharedFileNameSize+SHARED_FILE_EXTENSION_LENGTH+1;
    char *path = malloc (sizeof (char) * s);
    for(int i=0; i<s; i++){
        path[i]=(char)0;
    }
    char folder[] = "../sharedFiles/";
    strcat(path, folder);
    strcat(path, sharedFileName);
    strcat(path, SHARED_FILE_EXTENSION);
    return path;
}

char* getPathOfMetadataFile(char *torrentFileName, int sharedFileNameSize){
    //size = folderName size + sharedFileName size + extension size + 1
    int s = 18+sharedFileNameSize+4+1;
    char *path = malloc (sizeof (char) * s);
    for(int i=0; i<s; i++){
        path[i]=(char)0;
    }
    char folder[] = "../metadataFiles/";
    char extension[] = ".txt";
    strcat(path, folder);
    strcat(path, torrentFileName);
    strcat(path, extension);
    return path;
}

long long getSharedFileSizeInBytes(char *sharedPath){
    struct stat systemInformationAboutSharedFile;
    if(stat(sharedPath, &systemInformationAboutSharedFile) == -1){
        perror("stat");
        exit(1);
    }
    return (long long) systemInformationAboutSharedFile.st_size;
}

void insertName(FILE *metadataFile, char *fileName, long long int fileNameSize) {
    fprintf(metadataFile, "%s", "4:name");
    fprintf(metadataFile, "%lld", fileNameSize);
    fprintf(metadataFile, "%s", ":");
    fprintf(metadataFile, "%s", fileName);
}

void insertLength(FILE *metadataFile, long long int fileSize) {
    fprintf(metadataFile, "%s", "6:lengthi");
    fprintf(metadataFile, "%lld", fileSize);
    fprintf(metadataFile, "%s", "e");
}

void insertAnnounance(FILE *metadataFile, char *url, int size) {
    fprintf(metadataFile, "%s", "8:announce");
    fprintf(metadataFile, "%d", size);
    fprintf(metadataFile, "%s", ":");
    fprintf(metadataFile, "%s", url);
}

char* getPathOfFirstPiece(char *sharedFileName, int fileNameSize) {
    //size = folderName size + sharedFileName size + extension size + firstPieceLength + 1
    int s = 15+fileNameSize+SHARED_FILE_EXTENSION_LENGTH+2+1;
    char *path = malloc (sizeof (char) * s);
    for(int i=0; i<s; i++){
        path[i]=(char)0;
    }
    char folder[] = "../sharedFiles/";
    char firstPiece[] = "/1";
    strcat(path, folder);
    strcat(path, sharedFileName);
    strcat(path, firstPiece);
    strcat(path, SHARED_FILE_EXTENSION);
    return path;
}

void insertPieceLength(FILE *metadataFile, long long int pieceLength) {
    fprintf(metadataFile, "%s", "12:piece lengthi");
    fprintf(metadataFile, "%d", pieceLength);
    fprintf(metadataFile, "%s", "e");
}

void insertInfo(FILE *metadataFile, char *fileName, int fileNameSize, long long int fileSize) {
    fprintf(metadataFile, "%s", "4:infod");
    insertLength(metadataFile, fileSize);
    insertName(metadataFile, fileName, fileNameSize);

    char* firstPiecePath = getPathOfFirstPiece(fileName, fileNameSize);
    long long firstPieceSizeInBytes = getSharedFileSizeInBytes(firstPiecePath);
    insertPieceLength(metadataFile, firstPieceSizeInBytes);

//    insertPieces();TODO
    fprintf(metadataFile, "%s", "e");
}

const char *hexString(unsigned char *data, size_t length, char *buffer) {
    const char *hexDigits = "0123456789abcdef";
    char *dest = buffer;
    for (size_t i = 0; i < length; i++) {
        *dest++ = hexDigits[data[i] >> 4];
        *dest++ = hexDigits[data[i] & 0x0F];
    }
    *dest = 0;
    return buffer;
}

unsigned char* getMD5Hash(char *filename) {
    unsigned char *c= malloc (sizeof (unsigned char) * MD5_DIGEST_LENGTH);;
    int i;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        exit(1);
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);

    fclose (inFile);
    return c;
}

void printHash(unsigned char *hash){
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", hash[i]);
}

void createMetadataFile(char *sharedFileName, int sharedFileNameSize, char *trackerUrl, int trackerUrlSize ) {

    char *sharedPath = getPathOfSharedFile(sharedFileName, sharedFileNameSize);
    char *metadataPath = getPathOfMetadataFile(sharedFileName, sharedFileNameSize);

    long long sharedFileSizeInBytes = getSharedFileSizeInBytes(sharedPath);

    unsigned char *hash = getMD5Hash(sharedPath);

//    printHash(hash);



    FILE *metadataFile = fopen(metadataPath,"w");
    if(metadataFile == NULL){
        printf("Error!");
        exit(1);
    }
    fprintf(metadataFile, "%s", "d");
    insertAnnounance(metadataFile, trackerUrl, trackerUrlSize);
    insertInfo(metadataFile, sharedFileName, sharedFileNameSize, sharedFileSizeInBytes);
    fprintf(metadataFile, "%s", "e");

    fclose(metadataFile);


}
