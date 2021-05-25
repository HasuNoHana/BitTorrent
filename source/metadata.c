//
// Created by zuznanna on 25.05.2021.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char* getPathOfSharedFile(char *sharedFileName, int sharedFileNameSize){
    //size = folderName size + sharedFileName size + extension size + 1
    int s = 15+sharedFileNameSize+4+1;
    char *path = malloc (sizeof (char) * s);
    for(int i=0; i<s; i++){
        path[i]=(char)0;
    }
    char folder[] = "../sharedFiles/";
    char extension[] = ".txt";//TODO usun bo nazwa pliku bedzie z rozszerzeniem
    strcat(path, folder);
    strcat(path, sharedFileName);
    strcat(path, extension);
    return path;
}

char* getPathOfMetadataFile(char *torrentFileName, int sharedFileNameSize){
    //size = folderName size + sharedFileName size + extension size + 1
    int s = 18+sharedFileNameSize+5+1;
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

void createMetadataFile( char *sharedFileName, int size, char *trackerUrl ) {

    char *sharedPath = getPathOfSharedFile(sharedFileName, size);
    char *metadataPath = getPathOfMetadataFile(sharedFileName, size);

//    printf(sharedPath);
//    printf(metadataPath);

    long long sharedFileSizeInBytes = getSharedFileSizeInBytes(sharedPath);
//    printf("%lld\n", sharedFileSizeInBytes);

    FILE *metadataFile = fopen(metadataPath,"w");
    if(metadataFile == NULL){
        printf("Error!");
        exit(1);
    }

    fprintf(metadataFile,"%d",6);

    fclose(metadataFile);


}
