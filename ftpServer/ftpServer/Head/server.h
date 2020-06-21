#pragma once
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<memory>
#include<string>
#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT 16555    // 指定端口为16555
#define MAXLINK 2048
#define BUFFSIZE 2048
using namespace std;
class Server {

public:

	Server(SOCKET s);

	bool running();


private:

	SOCKET server;

	char Buff[BUFFSIZE];

	string storePath;


};