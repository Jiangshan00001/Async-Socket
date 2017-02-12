#ifndef DEF_SERVER
#define DEF_SERVER

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "Socket.h"

class Server
{
public:
    Server(int port);
    bool start();
    bool stop();
    Socket accept();

    void setErrorCallback(void (*callbackError)(std::string));

private:
    int sock;
    struct sockaddr_in server_params;

    void (*callbackError)(std::string);
    void reportError();
};

#endif
