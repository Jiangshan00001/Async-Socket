#include <curses.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <utility>
#include <sstream>
#include <iostream>

#include "../lib/Socket.h"

using namespace std;

void onClose(Socket sock)
{
    cout << "Disconnected" << endl;
}

void onError(Socket sock, string message)
{
    cout << "E: " << message << endl;
}

void socketCallback(Socket sock, string key, string message)
{
    cout << key << " : " << message << endl;
}

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        cout << "Syntax is :" << endl;
        cout << "client <ip> <port>" << endl;
        return 1;
    }
    string ip = argv[1];
    int port = atoi(argv[2]);

    Socket sock = Socket(ip, port);

    if(sock.connect()){
        cout << "Connected to server." << endl;

        sock.setCloseCallback(onClose);
        sock.setErrorCallback(onError);
        sock.setMessageCallback("message", socketCallback);

        if(sock.startReceiving()){
            string str;
            while(1){
                getline(cin, str);
                if(str!=""){
                    sock.send("message", str);
                }
            }
        }
    }
}
