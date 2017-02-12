#include "Server.h"

Server::Server(int port)
{
    server_params.sin_family = AF_INET;
    server_params.sin_addr.s_addr = INADDR_ANY;
    server_params.sin_port = htons(port);

    callbackError = 0;
}

bool Server::start()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock==-1)
    {
        reportError();
        return false;
    }

    if(bind(sock,(struct sockaddr *)&server_params , sizeof(server_params)) == -1)
    {
        reportError();
        return false;
    }

    if(listen(sock, 3) == -1){
        reportError();
        return false;
    }

    return true;
}

bool Server::stop()
{
    if(close(sock) == -1)
    {
        reportError();
        return false;
    }

    return true;
}

Socket Server::accept()
{
    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    int client_sock = ::accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);

    if(client_sock == -1){
        reportError();
        return Socket();
    }

    return Socket(client_sock);
}

void Server::setErrorCallback(void (*callbackError)(std::string))
{
    this->callbackError = callbackError;
}

void Server::reportError()
{
    if(callbackError!=0){
        callbackError(strerror(errno));
    }
}
