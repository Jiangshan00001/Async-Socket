#include "Socket.h"

Socket::Socket(std::string ip, int port)
{
    this->ip = ip;
    this->port = port;

    this->server.sin_addr.s_addr = inet_addr(ip.c_str());
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);

    this->sock = -1;

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

bool Socket::send(std::string key, std::string message)
{
    std::string msg = key+"+"+message;

    int n = htonl(msg.size());
    if(::send(sock, (const char*)&n, sizeOfHeader, 0) == -1){
        reportError();
        return false;
    }

    if(::send(sock, msg.c_str(), msg.size(), 0) == -1){
        reportError();
        return false;
    }

    return true;
}

bool Socket::write(std::string key, std::string message)
{
    std::string msg = key+"+"+message;

    int n = htonl(msg.size());
    if(::write(sock, (const char*)&n, sizeOfHeader) == -1){
        reportError();
        return false;
    }

    if(::write(sock, msg.c_str(), msg.size()) == -1){
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

void Socket::setMessageCallback(std::string key, void (*callback)(Socket, std::string, std::string))
{
    assert(key.find("+")==-1);
    this->callbackMessage.push_back(std::make_pair(key, callback));
}

void Socket::removeMessageCallback(std::string key)
{
    for (int i=0 ; i<callbackMessage.size() ; i++){
        if(callbackMessage[i].first == key){
            callbackMessage.erase(callbackMessage.begin()+i);
        }
    }
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

    for(int i=0 ; i<callbackMessage.size() ; i++){
        this->callbackMessage[i].second = 0;
    }
    this->callbackClose = 0;
    this->callbackError = 0;
    this->sock = -1;

    return true;
}

void Socket::receive()
{
    int sizeOfPackageNet;
    int sizeOfMessage;
    int q,r,i,n,p;
    std::stringstream finalMessage;
    std::string msg, key, body;

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

            msg = finalMessage.str();
            p = msg.find_first_of("+");
            key = msg.substr(0, p);
            body = msg.substr(p+1);

            for(i=0 ; i<callbackMessage.size() ; i++){
                if(callbackMessage[i].first==key){
                    callbackMessage[i].second(*this, key, body);
                }
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

int Socket::getSocket(){
    return sock;
}

bool Socket::operator==(Socket const &s)
{
    return(sock == s.sock);
}

bool Socket::operator!=(Socket const &s)
{
    return(sock != s.sock);
}
