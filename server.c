#include <stdio.h>  
#include <string.h>     
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> 
#include <signal.h>

#include "core.h"

#define TRUE 1


int serverCreate(struct sockaddr_in address, int port)
{
    int addrlen = sizeof(address); 
    int serverId;
    int opt = 1; 

    if ((serverId = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }  
    if (setsockopt(serverId, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( port ); 
       
    if (bind(serverId, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    return serverId;
}

int serverListen(int serverSocketId)
{
    if (listen(serverSocketId, 3) < 0) 
    { 
        perror("listen"); 
        return -1;
    }
    return 0;
}

void serverLoop(struct sockaddr_in address, int serverSocketId, char * buffer, int buffeSize)
{
    fd_set descriptionSet;
    int opt = TRUE;   
    int clientSockets[30], max_clients = 30;
    int activity;   
    int maxSocketDescriptor;
    int socketDescriptor;
    int addrlen = sizeof(address); 
    int newSocket;

    printf("Created server socket. IP : %s , Port : %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   

    for (int i = 0; i < max_clients; i++)   
    {   
        clientSockets[i] = 0;   
    } 
    while(TRUE)
    {
        FD_ZERO(&descriptionSet);   
        FD_SET(serverSocketId, &descriptionSet);   
        maxSocketDescriptor = serverSocketId;
        printf("Prepare clients %i \n", max_clients);   
        for (int i = 0 ; i < max_clients ; i++)   
        {   
            socketDescriptor = clientSockets[i];   
            if(socketDescriptor > 0)   
                FD_SET( socketDescriptor , &descriptionSet);
            if(socketDescriptor > maxSocketDescriptor)   
                maxSocketDescriptor = socketDescriptor;  
        }   
        activity = select( maxSocketDescriptor + 1 , &descriptionSet , NULL , NULL , NULL);   
        if ((activity < 0) && (errno!=EINTR))   
        {   
            perror("select");   
        }   
        if (FD_ISSET(serverSocketId, &descriptionSet))   
        {   
            if ((newSocket = accept(serverSocketId, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
            printf("New connection. IP : %s , Port : %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
            char * message = "Connected to server";
            if( send(newSocket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   
                 
            printf("Message sent successfully");   
                 
            for (int i = 0; i < max_clients; i++)   
            {   
                if( clientSockets[i] == 0 )   
                {   
                    clientSockets[i] = newSocket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   

        printf("After select read clients %i \n", max_clients);
        for (int i = 0; i < max_clients; i++)   
        {   
            socketDescriptor = clientSockets[i];   
                 
            if (FD_ISSET( socketDescriptor , &descriptionSet))   
            {   
                int valread;
                int bufferSize = sizeof(buffer);

                memset(buffer, 0, bufferSize);

                if (( valread = read( socketDescriptor , buffer, 1024)) == 0)   
                {   
                    getpeername(socketDescriptor , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                    close( socketDescriptor );   
                    clientSockets[i] = 0;   
                }   
                else 
                {   
                    printf("Read: %s\n", buffer);
                    buffer[valread] = '\0';   
                    strcat(buffer, " - Server");
                    send(socketDescriptor , buffer , strlen(buffer) , 0 );   
                }   
            }   
        }   
    }
}

int serverProcess() 
{ 
    char buffer[BUFFER_SIZE] = {0}; 
    char *hello = "Hello from server"; 
    struct sockaddr_in address;
    printf("Create \n");
    int serverSocketId = serverCreate(address, 8080);
    printf("Start Listen \n");
    serverListen(serverSocketId);
    printf("Loop \n");
    serverLoop(address, serverSocketId, buffer, BUFFER_SIZE);
    printf("End Loop \n");
    return 0; 
} 

int main(int argc, char const *argv[]) 
{
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
    serverProcess();
    return 0;
}
