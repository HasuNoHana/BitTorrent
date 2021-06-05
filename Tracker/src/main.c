#include "global_header.h"

File_S *head;               //Pointer to the head of file list.
int socketDescriptor;       //Tracker socket descriptor.
pthread_mutex_t lock;

char* getIP(char* str)
{
    int length = strlen(str);
    char* IP6 = malloc(IP_MAX);
    memset(IP6, 0, IP_MAX);
    
    for(int i=1; i<length && str[i]!=']'; i++)
    {
        IP6[i-1] = str[i];
    }

    return IP6;
}

char* getPort(char* str)
{
    int length = strlen(str);
    char* port = malloc(PORT_MAX);
    memset(port, 0, PORT_MAX);
    
    int i=0;
    for(; i<length && str[i]!=']'; i++){}
    
    i+=2;
    for(int j=0; j<PORT_MAX && i<length; j++, i++)
    {
        port[j]=str[i];
    }

    return port;
}

/*
 *  argv[1] - Traker IP and Port Number [<ip>]:<port>,
 *  argv[2] - Path to the file that contains file list
 */
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        perror("Too few arguments for main function. \n");
        return 1;
    }
    
    char* mytracker_IP_port = argv[1];
    char* seeder_list = argv[2];

    char* mytrackerIP = getIP(mytracker_IP_port);
    char* mytrackerPort = getPort(mytracker_IP_port);
    
    struct sockaddr_in6 clientAddress;
    int clientAddrlen = sizeof(clientAddress);

    printf("Tracker is running.\nTracker IP: %s, Port: %s\n", mytrackerIP, mytrackerPort);

    head = (File_S *) malloc(sizeof(File_S));

    loadSeedersFromInitialFile(seeder_list);

    printf("\nLoaded Files:\n");
    loopSeeders();
    
    createSocket(mytrackerIP, mytrackerPort);
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("[-] Mutex init failed.\n");
        exit(EXIT_FAILURE);
    }

    while (1){
        int client_socket;
        char clientIP[INET6_ADDRSTRLEN];
        int thread;
        pthread_t threadID;
        ThreadArgs* args = (ThreadArgs *) malloc(sizeof(ThreadArgs));

        if ((client_socket = accept(socketDescriptor, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddrlen)) < 0){
            perror ("[-] Accept() error.");
            exit(EXIT_FAILURE);
        }
        printf("Connected");

        struct in6_addr in6addr = clientAddress.sin6_addr;
    
        if (inet_ntop(AF_INET6, &in6addr, clientIP, sizeof(clientIP)) != NULL) printf(" to %s\n", clientIP);
        
        args->client_socket = client_socket;
        args->clientIP = clientIP;

        pthread_create(&threadID, NULL, serveRequest, (void *)args);
        thread = pthread_detach(threadID);
    }

    File_S* current = head;
    File_S* next;
 
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
   
    head = NULL;

    pthread_mutex_destroy(&lock);
    return 0;
}