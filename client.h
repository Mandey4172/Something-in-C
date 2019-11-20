int clientConnect(char * address, int port);
int clientSendMessage(int socketId, char * message);
int clientReciveMessage(int socketId, char * buffer, int bufferSize);
int clientProcess(void);
