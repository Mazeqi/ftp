#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<memory>
#include<string>
#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT 16555    // 指定端口为16555
#define MAXLINK 2048
#define BUFFSIZE 1024

using namespace std;
class Server {

public:

	Server(SOCKET s);

	bool running();

	bool RecvFile(string filename);

	void WelCome();
private:

	SOCKET server;

	char Buff[BUFFSIZE];

	string storePath;


};