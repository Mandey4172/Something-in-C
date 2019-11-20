#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#include "core.h"
#include "client.h"

int clientConnect(char * address, int port)
{
    int socketId = 0; 
    struct sockaddr_in serv_addr; 

    if ((socketId = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        return -1; 
    } 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)  
    { 
        return -1; 
    } 
    if (connect(socketId, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        return -1; 
    }
    return socketId; 
}

int clientSendMessage(int socketId, char * message)
{
   return send(socketId , message , strlen(message) , 0 ); 
}

int clientReciveMessage(int socketId, char * buffer, int bufferSize)
{
    int size = read(socketId , buffer, bufferSize); 
    if(size > 0)
        buffer[size] = '\0';
}


int clientDisconnect(int socketId)
{
    return close(socketId);
}

int main(int argc, char const *argv[])
{
    char *ip = "127.0.0.1";
    int port = 8080;
    // int readSize = 0;
    char buffer[BUFFER_SIZE] = {0};
    char *message = "Client message";

    int socketId = clientConnect(ip, port);
    printf("Connected\n");

    // readSize = read(socketId, buffer, bufferSize);
    // buffer[readSize] = '\0';
    clientReciveMessage(socketId, buffer, sizeof(buffer));
    printf("Message from server: %s\n", buffer);

    // send(socketId, message, strlen(message), 0);
    clientSendMessage(socketId, message);
    printf("Message send: %s\n", message);

    // readSize = read(socketId , buffer, bufferSize);
    // buffer[readSize] = '\0';
    clientReciveMessage(socketId, buffer, sizeof(buffer));
    printf("Message from server: %s\n", buffer);

    clientDisconnect(socketId);
    printf("Disconnected\n");
}
