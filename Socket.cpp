#include "Socket.h"

Socket::Socket(std::string ip, int port)
{
    this->ip = ip;
    this->port = port;

    this->server.sin_addr.s_addr = inet_addr(ip.c_str());
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);

    init();
}

Socket::Socket(int sock)
{
    this->sock = sock;
    init();
}

Socket::Socket()
{
    this->sock = -1;
    init();
}

void Socket::init()
{
    this->callbackMessage = 0;
    this->callbackClose = 0;
    this->callbackError = 0;
    this->sizeOfHeader = 32;
    this->sizeOfPackage = 4096;
}

bool Socket::connect()
{
    sock = socket(AF_INET , SOCK_STREAM , 0);

    if(sock==-1){
        reportError();
        return false;
    }

    if(::connect(sock , (struct sockaddr *)&server , sizeof(server)) == -1)
    {
        reportError();
        return false;
    }

    return true;
}

bool Socket::send(std::string message)
{
    int n = htonl(message.size());
    if(::send(sock, (const char*)&n, sizeOfHeader, 0) == -1){
        reportError();
        return false;
    }

    if(::send(sock, message.c_str(), message.size(), 0) == -1){
        reportError();
        return false;
    }

    return true;
}

bool Socket::write(std::string message)
{
    int n = htonl(message.size());
    if(::write(sock, (const char*)&n, sizeOfHeader) == -1){
        reportError();
        return false;
    }

    if(::write(sock, message.c_str(), message.size()) == -1){
        reportError();
        return false;
    }

    return true;
}

bool Socket::startReceiving()
{
    pthread_t thread_id;
    return pthread_create(&thread_id, NULL, Socket::start_thread, this) == 0;
}

void Socket::setReceiveCallback(void (*callbackMessage)(Socket, std::string))
{
    this->callbackMessage = callbackMessage;
}

void Socket::setCloseCallback(void (*callbackClose)(Socket))
{
    this->callbackClose = callbackClose;
}

void Socket::setErrorCallback(void (*callbackError)(Socket, std::string))
{
    this->callbackError = callbackError;
}

void Socket::reportError()
{
    if(callbackError!=0){
        callbackError(*this, strerror(errno));
    }
}

bool Socket::close()
{
    if(::close(sock) == -1)
    {
        reportError();
        return false;
    }

    this->callbackMessage = 0;
    this->callbackClose = 0;
    this->callbackError = 0;

    return true;
}

void Socket::receive()
{
    int sizeOfPackageNet;
    int sizeOfMessage;
    int q,r,i,n;
    std::stringstream finalMessage;

    while (1) {
        n = ::recv(sock, &sizeOfPackageNet, sizeOfHeader, 0);
        if(n > 0){
            sizeOfMessage = ntohl(sizeOfPackageNet);
            q = sizeOfMessage/sizeOfPackage;
            r = sizeOfMessage%sizeOfPackage;

            char *message = new char[sizeOfPackage];
            for (i=0 ; i<q ; i++)
            {
                n = ::recv(sock, message, sizeOfPackage, 0);
                if(n > 0){
                    finalMessage << std::string(message, sizeOfPackage);
                    memset(message, 0, sizeOfPackage);
                }
                else if(n==0){
                    if(callbackClose!=0){
                        callbackClose(*this);
                    }
                    pthread_exit(NULL);
                }
                else{
                    reportError();
                }
            }
            delete[] message;

            if(r!=0)
            {
                char *rest = new char[r];
                n = ::recv(sock, rest, r, 0);
                if(n > 0){
                    finalMessage << std::string(rest, r);
                }
                else if(n==0){
                    if(callbackClose!=0){
                        callbackClose(*this);
                    }
                    pthread_exit(NULL);
                }
                else{
                    reportError();
                }
                delete[] rest;
            }

            if(callbackMessage!=0){
                callbackMessage(*this, finalMessage.str());
            }

            finalMessage.clear();
            finalMessage.str(std::string());
        }
        else if(n==0){
            if(callbackClose!=0){
                callbackClose(*this);
            }
            pthread_exit(NULL);
        }
        else{
            reportError();
        }
        usleep(100);
    }
}

bool Socket::operator==(Socket const &s)
{
    return(sock == s.sock);
}

bool Socket::operator!=(Socket const &s)
{
    return(sock != s.sock);
}
