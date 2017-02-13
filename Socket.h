#ifndef DEF_SOCKET
#define DEF_SOCKET

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <sstream>

class Socket
{
public:
    Socket(std::string ip, int port);
    Socket(int sock);
    Socket();

    bool connect();
    bool close();
    bool send(std::string message);
    bool write(std::string message);
    bool startReceiving();
    void setReceiveCallback(void (*callbackMessage)(Socket, std::string));
    void setCloseCallback(void (*callbackClose)(Socket));
    void setErrorCallback(void (*callbackError)(Socket, std::string));

    bool operator==(Socket const &s);
    bool operator!=(Socket const &s);

private:
    std::string ip;
    int port;
    int sock;
    struct sockaddr_in server;

    int sizeOfHeader;
    int sizeOfPackage;

    void (*callbackMessage)(Socket, std::string);
    void (*callbackClose)(Socket);
    void (*callbackError)(Socket, std::string);

    void reportError();
    void receive();
    void init();

    static void *start_thread(void *context)
    {
        ((Socket*)context)->receive();
        return NULL;
    }
};

#endif
