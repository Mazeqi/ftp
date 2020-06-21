#include "../Head/server.h"

#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
//void server(SOCKET s);
Server::Server(SOCKET s):server(s) 
{
	storePath = ".//file//";
}

bool Server::running() {

}