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
#include <vector>
#include <utility>

class Socket
{
public:
    Socket(std::string ip, int port);
    Socket(int sock);
    Socket();

    bool connect();
    bool close();
    bool send(std::string key, std::string message);
    bool write(std::string key, std::string message);
    bool startReceiving();
    void setMessageCallback(std::string key, void (*callback)(Socket, std::string, std::string));
    void removeMessageCallback(std::string key);
    void setCloseCallback(void (*callbackClose)(Socket));
    void setErrorCallback(void (*callbackError)(Socket, std::string));

    int getSocket();

    bool operator==(Socket const &s);
    bool operator!=(Socket const &s);

private:
    std::string ip;
    int port;
    int sock;
    struct sockaddr_in server;

    int sizeOfHeader;
    int sizeOfPackage;

    std::vector<std::pair<std::string, void (*)(Socket, std::string, std::string)> > callbackMessage;
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
