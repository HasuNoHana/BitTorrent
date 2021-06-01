#include "global_header.h"


void insertPiece(char* filename, char* line, char* clientIP, int isOriginal, int readLine)
{
    File_S* current = head;
    Piece* current_piece;
    Seed* current_seed;

    int length = strlen(line);
    char* piece = malloc(PIECE_LENGTH);
    char* seedIP = clientIP;

    int i=1;

    while (current->next != NULL){
        if(strcmp(current->filename, filename)==0) break; 
        else current = current->next;
    }

    if(readLine == 1) for(;i<length-1; i++) *(piece+i-1) = line[i];  
    else if(readLine==0 && current->next == NULL) return;

    if(current->next == NULL){
        current->filename = filename;
        
        current->piece_head=(Piece *) malloc(sizeof(Piece));
        current_piece = current->piece_head;
        current_piece->pieceHash=piece;
        current_piece->next=(Piece *) malloc(sizeof(Piece));
        current_piece->next->next = NULL;

        current->seed_head =(Seed *) malloc(sizeof(Seed));
        current_seed = current->seed_head;
        current_seed->seedIP=seedIP;
        current_seed->isOriginal=isOriginal;
        current_seed->next=(Seed *) malloc(sizeof(Seed));;

        current->next = (File_S*) malloc(sizeof(File_S));
        current->next->next = NULL;
    } 
    else{        
        current_piece = current->piece_head;
        if(readLine==0) piece = current->piece_head->pieceHash;
        
        while (current_piece->next != NULL){
            if (strcmp(current_piece->pieceHash, piece)==0) break;
            else current_piece = current_piece->next; 
        }
        
        if (current_piece->next == NULL){
            current_piece->pieceHash = piece;
            current_piece->next=(Piece *) malloc(sizeof(Piece));
            current_piece->next->next=NULL;
        }

        current_seed = current->seed_head;

        while (current_seed->next != NULL){
            if (strcmp(current_seed->seedIP, seedIP)==0) break;
            else current_seed = current_seed->next; 
        }
        
        if (current_seed->next == NULL){
            current_seed->seedIP = seedIP;
            current_seed->isOriginal=isOriginal;
            current_seed->next=(Seed *) malloc(sizeof(Seed));
            current_seed->next->next=NULL;
        }
    }
    return;
}

int checkFile(char* filename)
{
    File_S* current = head;

    while (current->next != NULL){
        if(strcmp(current->filename, filename)==0) break; 
        else current = current->next;
    }

    if(current->next == NULL) return 0;
    else return 1;
}

void loopSeeders()
{
    File_S* current = head;
    Piece* current_piece;
    Seed* current_seed;

    while (current->next != NULL) {
        printf("\n%s:\n", current->filename);
        printf(" pieces:\n");

        current_piece = current->piece_head;
        current_seed = current->seed_head;
        
        while (current_piece->next != NULL){
            printf("\t%s\n", current_piece->pieceHash);
            current_piece = current_piece->next;
        }
        
        printf(" seeds: \n");

        while (current_seed->next != NULL){
            printf("\t%s %d\n", current_seed->seedIP, current_seed->isOriginal);
            current_seed = current_seed->next;
        }

        current = current->next;
    }
    printf("\n");
    return;
}


int accessFilesList(int mode, char* filename, char* filepath, char* clientIP)
{
    pthread_mutex_lock(&lock);

    if(mode == 1 ){
        FILE* fp = getFilePointer(2, filepath); 
        char* filename=NULL;
        char* line_buf = NULL;
        size_t line_buf_size = 0;
        ssize_t line;

        while (line = getline(&line_buf, &line_buf_size, fp) > 0 && (strncmp(line_buf, "6:pieces", 6)!=0))
            if (strncmp(line_buf, "4:name", 6)==0) filename = getFilename(line_buf);

        if(checkFile(filename)==1)
        {
            insertPiece(filename, line_buf, clientIP, 0, 0);
            pthread_mutex_unlock(&lock);
            return 0;
        }
        else
            while (line = getline(&line_buf, &line_buf_size, fp) > 0 && (strncmp(line_buf, "e", 1)!=0)) 
                insertPiece(filename, line_buf, clientIP, 1, 1);
        
        line_buf = NULL;
        filename = NULL;
        free(line_buf);
        free(filename);
        fclose(fp);
    }
    else if(mode == 2 ){   
        File_S* current = head;
        Piece* current_piece;
        Seed* current_seed;

        while(current->next != NULL){
            if(strcmp(current->filename, filename)==0) break;
            else current = current->next;
        }

        if(current->next == NULL) 
        {
            pthread_mutex_unlock(&lock);
            return 0;
        }
        else{
            FILE* fp = getFilePointer(1, filepath);   
            
            current_piece = current->piece_head;
            current_seed = current->seed_head;
            
            fputs("pieces:\n", fp);

            while (current_piece->next != NULL){
                fputs(current_piece->pieceHash, fp);
                fputc('\n', fp);
                current_piece = current_piece->next;
            }

            fputs("seeds:\n", fp);

            while (current_seed->next != NULL){
                fputs(current_seed->seedIP, fp);
                fputc('\n', fp);
                current_seed = current_seed->next;
            }

            fclose(fp);
        }
    }else if(mode == 3 )
    {   
        File_S* current = head;
        Piece* current_piece;
        Seed* current_seed;
        File_S* prev = NULL;
        Seed* prev_seed = NULL;
        int wasOriginal = 0;

        while(current->next != NULL){
            if(strcmp(current->filename, filename)==0) break;
            else{   
                prev = current;
                current = current->next;
            }
        }

        if(current->next == NULL){
            pthread_mutex_unlock(&lock);
            return 0;
        }
        else{         
            current_seed = current->seed_head;
            if (current_seed != NULL && (strcmp(current_seed->seedIP, clientIP) == 0)){
                current->seed_head = current_seed->next; 
                free(current_seed);  
            }
            else{
                while (current_seed->next != NULL){
                    if(strcmp(current_seed->seedIP, clientIP) == 0) break;
                    else{
                        prev_seed = current_seed;
                        current_seed = current_seed->next;
                    }
                }

                if (current_seed->next == NULL){
                    pthread_mutex_unlock(&lock);
                    return 2;
                }
                else{
                    if (current_seed->isOriginal == 1) wasOriginal = 1;
                    prev_seed->next = current_seed->next;

                    free(current_seed);
                }
            }

            current_seed = current->seed_head;

            if (current->seed_head->next == NULL){

                if (prev == NULL){
                    current = current->next; 
                    free(current);  
                }
                else{                    
                    prev->next = current->next;
                    free(current);
                }
            }
            else if(wasOriginal == 1){
                current->seed_head->isOriginal == 1;
            }
        } 
    }
    else if(mode == 4)
    {   
        File_S* current = head;
        Seed* current_seed;

        while(current->next != NULL){
            if(strcmp(current->filename, filename)==0) break;
            else current = current->next;
        }

        if(current->next == NULL) 
        {   
            pthread_mutex_unlock(&lock);
            return 0;
        }
        else{         
            current_seed = current->seed_head;

            while (current_seed->next != NULL){
                if (strcmp(current_seed->seedIP, clientIP)==0) break;
                else current_seed = current_seed->next; 
            }
        
            if (current_seed->next == NULL){
                current_seed->seedIP = clientIP;
                current_seed->isOriginal=0;
                current_seed->next=(Seed *) malloc(sizeof(Seed));
                current_seed->next->next=NULL;
            }
            else{
                pthread_mutex_unlock(&lock);
                return 2;
            }
        }
    }
    pthread_mutex_unlock(&lock);
    return 1;
}

