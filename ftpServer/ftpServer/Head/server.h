#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<memory>
#include<string>
#include<string>
#include<vector>
#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT 16555    // 指定端口为16555
#define MAXLINK 2048
#define BUFFSIZE 1024

enum CMD {
	ERR = 0,
	USER = 1,
	PASS = 2,
	DIR = 3
};

using namespace std;
class Server {

public:

	Server(SOCKET s);

	bool running();

	bool RecvFile(string filename);

	void WelCome();

	CMD commandParse(vector<string> &strVec, string command);

	//检查sock是否还活着
	bool checkSock(int scok);

	void User(vector<string> strVec);

	void Pass(vector<string> strVec);

	void fileDir();

	bool confirmStatus();

	void sendERR(string err, int errStatus, string sendErr, int sendErrStatus);


private:

	SOCKET server;

	char Buff[BUFFSIZE];

	string storePath;

	//当这两个状态某一个为0时，无法使用其他指令。
	bool USER_Status;

	bool PASS_Status;

	//与服务器连接的客户端名
	string Client_name;
	
	//与服务器连接的客户端ip
	string Client_ip;

	//与服务器连接的客户端端口
	int Client_Port;


};