#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<vector>
#include<string>
#pragma comment(lib,"Ws2_32.lib")

#define BUFFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16555

using namespace std;

 enum CMD{
	ERR = 0,
	USER = 1,
	PASS = 2,
	ServerDIR = 3,
	CliDIR = 4
} ;
class Client {

public:

	~Client();

	bool running();

	bool sendFile(string filePath);

	bool recvFile();

	void menu();

	void User(vector<string> strVec);

	CMD commandParse(vector<string> &vec, string command);

	bool sendCommand(string command);

	void cliDir();

	void serverDir();

private:

	SOCKET sockfd;

	char Buff[BUFFSIZE];
};

