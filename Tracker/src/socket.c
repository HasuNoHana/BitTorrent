#include "global_header.h"

struct sockaddr_in6 address;
int addrlen;

void createSocket(const char* mytrackerIP, const char* mytrackerPort)
{
    int opt = 1;
    struct in6_addr in6addr;
    int rval;

    if ((socketDescriptor = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
        perror("[-] Socket() error.\n");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("[-] Setsockopt() error.\n");
        exit(EXIT_FAILURE);
    }

    address.sin6_family = AF_INET6;
    rval = inet_pton(AF_INET6, mytrackerIP, &in6addr);
    address.sin6_addr = in6addr; 
    address.sin6_port = htons(atoi(mytrackerPort));

    if (bind(socketDescriptor, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("[-] Bind() error.\n");
        exit(EXIT_FAILURE);
    }

    if (listen(socketDescriptor, SOMAXCONN) < 0){
        perror("[-] Listen() error\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Listening at [%s]:%s \n", mytrackerIP, mytrackerPort);
}


void *serveRequest(void *arguments)
{
    ThreadArgs *args = (ThreadArgs *)arguments;
    int client_socket = args->client_socket;
    char* clientIP = args->clientIP;
    
    char buffer_in[FILEN_LENGTH];
    char* buffer_out = malloc(55);

    read(client_socket, buffer_in, FILEN_LENGTH);

    printf("%s - option %s\n", clientIP, buffer_in);

    if (buffer_in[0] == '1'){   
        buffer_out = "Hi, want to add new file? Send me torrent file.\n";
        write(client_socket, buffer_out, 48);

        memset(buffer_in, 0, FILEN_LENGTH);
        
        addFile(client_socket, clientIP);
    }
    else if (buffer_in[0] == '2')
    {
        buffer_out = "Hi, want to download file? Send me it's name.\n";
        write(client_socket, buffer_out, 46);
        
        memset(buffer_in, 0, FILEN_LENGTH);
        read(client_socket, buffer_in, FILEN_LENGTH);

        download(client_socket, clientIP, buffer_in);
    }
    else if (buffer_in[0] == '3'){
        buffer_out = "Hi, want to delete file? Send me it's name.\n";
        write(client_socket, buffer_out, 44);

        memset(buffer_in, 0, FILEN_LENGTH);
        read(client_socket, buffer_in, FILEN_LENGTH);
        
        removeSeeder(client_socket, clientIP, buffer_in);
    }
    else if (buffer_in[0] == '4')
    {
        buffer_out = "Hi, finished downloading a file? Send me it's name.\n";
        write(client_socket, buffer_out, 52);

        memset(buffer_in, 0, FILEN_LENGTH);
        read(client_socket, buffer_in, FILEN_LENGTH);

        updateSeedersList(client_socket, clientIP, buffer_in);
    }
    else
    {
        buffer_out = "Unknown command\n";
        write(client_socket, buffer_out, 16);
        close(client_socket);
    }
    return NULL;
}

char* receiveFile(int client_socket, char* clientIP)
{
    int n;
    char *filepath = malloc(12+IP_MAX);
    memset(filepath, 0, 12+IP_MAX);

    strcat(filepath, "torrent_");
    strcat(filepath, clientIP);
    strcat(filepath, ".txt");
        
    FILE *fp = getFilePointer(1, filepath);

    char buffer[DATASIZE];

    while (1){
        n = recv(client_socket, buffer, DATASIZE, 0);
        if (n <= 0) break;

        fprintf(fp, "%s", buffer);
        bzero(buffer, DATASIZE);
    }

    return filepath;
}

void sendFile(FILE *fp, int client_socket){
    int n;
    char* data = malloc(DATASIZE);

    while(fgets(data, DATASIZE, fp) != NULL) {
        printf("%s", data);

        if (send(client_socket, data, sizeof(DATASIZE), 0) == -1){
            perror("[-] Error in sending file.");
            exit(1);
        }

        bzero(data, DATASIZE);
    }
    return;
}
