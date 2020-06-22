#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#pragma comment(lib,"Ws2_32.lib")

#define BUFFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

using namespace std;

class Client {

public:
	bool running();

	bool sendFile(string filePath);

	bool recvFile();

private:
	SOCKET sockfd;

	char Buff[BUFFSIZE];
};

