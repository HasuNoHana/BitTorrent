#include "global_header.h"

void addFile(int client_socket, char* clientIP)
{ 
    char* buffer_out = malloc(70);
    
    //Nie mam pojecia czy ta funkcja dziala:
    //char* filepath = receiveFile(client_socket, clientIP);
    
    char* filepath = "torrent.txt";

    int success = accessFilesList(1, NULL, filepath, clientIP);

    if (success==1){
        printf("New file added. Updated list: \n");
        loopSeeders();

        buffer_out = "File was succesfully added to the list.\n";
        write(client_socket, buffer_out, 41);
        close(client_socket);
    }
    if (success==0){
        buffer_out = "This file already exists, you've been added to file owners list.\n";
        
        printf("New file owner added. Updated list: \n");
        loopSeeders();

        write(client_socket, buffer_out, 67);
        close(client_socket);
    }
    return;
}

void download(int client_socket, char* clientIP, char* filename)
{
    char* buffer_out = malloc(70);
    char* trimmed_filename = trimwhitespace(filename);

    char filepath[8+FILEN_LENGTH+IP_MAX];
    memset(filepath, 0, 8+FILEN_LENGTH+IP_MAX);

    strcat(filepath, "seeds_");
    strcat(filepath, trimmed_filename);
    strcat(filepath, "[");
    strcat(filepath, clientIP);
    strcat(filepath, "]");

    int success = accessFilesList(2, trimmed_filename, filepath, clientIP);

    if (success == 0){
        buffer_out = "I don't have this file\n";
        write(client_socket, buffer_out, 23);

        close(client_socket);
    }
    if (success == 1){
        buffer_out = "Seeder's file is prepared, sending will start after this message.\n";
        write(client_socket, buffer_out, 67);

        FILE* fp = getFilePointer(2, filepath);   
        
        sendFile(fp, client_socket);

        printf("File %s sent to %s.\n", filepath, clientIP);        
        fclose(fp);
        close(client_socket);
    }
    return;
}

void removeSeeder(int client_socket, char* clientIP, char* filename)
{
    char* buffer_out = malloc(72);
    
    char* trimmed_filename = trimwhitespace(filename);

    int success = accessFilesList(3, trimmed_filename, NULL, clientIP);

    if (success == 0){
        buffer_out = "I don't have this file\n";
        write(client_socket, buffer_out, 23);
        close(client_socket);
    }
    else if (success == 1){
        buffer_out = "You were successfully ereased from this file's owners list.\n";

        printf("File owner ereased. Updated list: \n");
        loopSeeders();

        write(client_socket, buffer_out, 71);

        close(client_socket);
    }
    else if (success == 2){
        buffer_out = "You are not in this file owners list.\n";
        write(client_socket, buffer_out, 38);
        close(client_socket);
    }
    return;
}


void updateSeedersList(int client_socket, char* clientIP, char* filename)
{
    char* buffer_out = malloc(80);
    char* trimmed_filename = trimwhitespace(filename);

    int success = accessFilesList(4, trimmed_filename, NULL, clientIP);

    if (success == 0){
        buffer_out = "I don't have this file\n";
        write(client_socket, buffer_out, 23);
        close(client_socket);
    }
    else if (success == 1){
        buffer_out = "You were successfully added to this file's owners list.\n";
        
        printf("New file owner added. Updated list: \n");
        loopSeeders();

        write(client_socket, buffer_out, 55);
        close(client_socket);
    }
    else if (success == 2){
        buffer_out = "You downloaded this file before, you already exist on this file owners list.\n";
        write(client_socket, buffer_out, 76);
        close(client_socket);
    }
}

char *trimwhitespace(char *str)
{
    char *end;
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) 
    return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}