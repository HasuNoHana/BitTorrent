#include "global_header.h"

FILE* getFilePointer(int mode, char* filepath)
{
    FILE* fp;

    if (mode == 1){
        fp = fopen(filepath, "w");
        
        if(!fp) fprintf(stderr, "Error creating and opening file '%s'\n", filepath);
    } 
    if (mode == 2){
        fp = fopen(filepath, "r");
        
        if(!fp) fprintf(stderr, "Error opening file '%s'\n", filepath);
    }

    return fp;
}

void loadSeedersFromInitialFile(char* filepath)
{
    FILE* fp = getFilePointer(2, filepath);   
    char *filename;
    char *seedIP;

    char* line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line;
    
    while ((line = getline(&line_buf, &line_buf_size, fp)) > 0){

        if(strncmp(line_buf, "4:name", 6)==0) filename = getFilename(line_buf);
        else if (strncmp(line_buf, "5:owner", 7)==0) seedIP = getOwnerIP(line_buf);
        else if (strncmp(line_buf, "6:pieces", 6)==0){
            while(line = getline(&line_buf, &line_buf_size, fp) > 0 && (strncmp(line_buf, "7:seeds", 7)!=0)) 
                insertPiece(filename, line_buf, seedIP, 1, 1);
    
            while((line = getline(&line_buf, &line_buf_size, fp) > 0) && (strncmp(line_buf, ";", 1)!=0)){
                seedIP = getSeedIP(line_buf);
                insertPiece(filename, "anything", seedIP, 0, 0);
            }   
        }
    }

    free(line_buf);
    fclose(fp);

    printf("Seeders successfully loaded from initial file. \n");   
    return;
}

char* getFilename(char* line)
{
    int length = strlen(line);
    char* filename = malloc(FILEN_LENGTH);
    memset(filename, 0, FILEN_LENGTH);

    int i=2;
    for(;i<length && line[i]!=':'; i++){}

    i++;
    for(int j=0; j<FILEN_LENGTH, i<length-1; i++, j++) *(filename+j) = line[i];
   
    return filename;  
}

char* getOwnerIP(char* line)
{
    int length = strlen(line);
    char* seedIP = malloc(IP_MAX);
    memset(seedIP, 0, IP_MAX);

    int i=2;
    for(;i<length && line[i]!=':'; i++){}

    i++;
    for(int j=0; j<IP_MAX, i<length-1; i++, j++) *(seedIP+j) = line[i];

    return seedIP;  
}

char* getSeedIP(char* line)
{
    int length = strlen(line);
    char* seedIP = malloc(IP_MAX);
    memset(seedIP, 0, IP_MAX);

    for(int j=0, i=0; j<IP_MAX, i<length-1; i++, j++) *(seedIP+j) = line[i];

    return seedIP;  
}
