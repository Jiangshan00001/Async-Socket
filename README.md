# Async-Socket
Asynchronous socket class implemented in C++ for Linux systems.

## Server example

    void onMessage(Socket sock, string message)
    {
        // Socket 'sock' sent 'message'
    }

    void onClose(Socket sock)
    {
        // Socket 'sock' has been disconnected
    }

    void onClientError(Socket sock, string message)
    {
        // Error occurred on client socket
    }

    void onServerError(string message)
    {
        // Error occurred on server socket
    }

    int main()
    {
        Server server(5000);
        server.setErrorCallback(onServerError);

        if(server.start()){
            cout << "Server started" << endl;

            Socket client = server.accept();
            client.setReceiveCallback(onMessage);
            client.setCloseCallback(onClose);
            client.setErrorCallback(onClientError);
            client.startReceiving(); // start a thread to listen for incoming messages

            cout << "client connected" << endl;

            /*
            ... loop or something ...
            */
        }
        else{
            cout << "Could not start server" << endl;
        }
    }

### Compile

    g++ server.cpp -o server Socket.h Socket.cpp Server.h Server.cpp -pthread

## Client example

    void onMessage(Socket sock, string message)
    {
        // Socket 'sock' received 'message'
    }

    void onClose(Socket sock)
    {
        // Socket 'sock' has been disconnected
    }

    void onError(Socket sock, string message)
    {
        // Error occurred
    }

    int main()
    {
        Socket sock("127.0.0.1", 5000);
        if(sock.connect()){
            cout << "Connected" << endl;

            sock.setReceiveCallback(onMessage);
            sock.setCloseCallback(onClose);
            sock.setErrorCallback(onError);
            sock.startReceiving(); // start a thread to listen for incoming messages

            string msg;
            while(1)
            {
                cout << "input: ";
                getline(cin, msg);
                sock.send(msg);
            }
        }
        else{
            cout << "Could not connect to server" << endl;
        }
    }

### Compile

    g++ client.cpp -o client Socket.h Socket.cpp -pthread
