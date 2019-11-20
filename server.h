int serverCreate(struct sockaddr_in address, int port);

int serverListen(int serverSocketId);

void serverLoop(struct sockaddr_in address, int serverSocketId, char * buffer, int buffeSize);

int serverProcess();