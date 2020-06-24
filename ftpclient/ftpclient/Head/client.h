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
	SPACE = -1,
	ERR = 0,
	USER = 1,
	PASS = 2,
	ServerDIR = 3,
	CliDIR = 4,
	GET = 5,
	PUT = 6,
} ;
class Client {

public:
	Client();

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

	void Get(vector<string> strVec);

private:

	SOCKET sockfd;

	char Buff[BUFFSIZE];

	string storePath;
};

