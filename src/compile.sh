#! /bin/sh

g++ client.cpp ../lib/Socket.h ../lib/Socket.cpp -o client -lncurses -pthread
g++ server.cpp ../lib/Server.h ../lib/Server.cpp ../lib/Socket.h ../lib/Socket.cpp -o server -pthread
