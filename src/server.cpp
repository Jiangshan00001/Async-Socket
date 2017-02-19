#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../lib/Server.h"

using namespace std;

vector<Socket> clients;

int getId(Socket sock)
{
    for(int i=0 ; i<clients.size() ; i++)
    {
        if(sock==clients[i]){
            return i;
        }
    }
    return -1;
}

int getAvailableId()
{
    for (int i=0 ; i<clients.size() ; i++)
    {
        if(clients[i].getSocket()==-1){
            return i;
        }
    }

    clients.push_back(Socket());
    return clients.size()-1;
}

void forward(Socket except, string key, string message)
{
    for (int i=0 ; i<clients.size() ; i++)
    {
        if(clients[i]!=except && clients[i].getSocket()!=-1){
            clients[i].write(key, message);
        }
    }
}

void onError(Socket sock, string message)
{
    cout << "E: " << message << endl;
}

void onServerError(string message)
{
    cout << "E: " << message << endl;
}

void onClose(Socket sock)
{
    int id = getId(sock);
    if(id != -1){
        clients[id].close();
        cout << "Socket[" << id << "] disconnected" << endl;
    }
}

void onMessage(Socket sock, string key, string message)
{
    forward(sock, key, message);
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        cout << "Syntax is :" << endl;
        cout << "server <port>" << endl;
        return 1;
    }
    int port = atoi(argv[1]);

    Server server(port);
    server.setErrorCallback(onServerError);

    if(server.start()){
        cout << "Server started on port " << port << endl;

        Socket sock;
        int id;

        while(1)
        {
            sock = server.accept();
            id = getAvailableId();

            clients[id] = sock;
            clients[id].setErrorCallback(onError);
            clients[id].setCloseCallback(onClose);
            clients[id].setMessageCallback("message", onMessage);
            clients[id].setMessageCallback("key", onMessage);
            clients[id].startReceiving();

            cout << "Socket[" << id << "] connected : " << clients[id].getSocket() << endl;
        }
    }

    server.stop();
}
