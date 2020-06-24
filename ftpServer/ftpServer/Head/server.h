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

#define DEFAULT_PORT 16555    // ָ���˿�Ϊ16555
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

	//���sock�Ƿ񻹻���
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

	//��������״̬ĳһ��Ϊ0ʱ���޷�ʹ������ָ�
	bool USER_Status;

	bool PASS_Status;

	//����������ӵĿͻ�����
	string Client_name;
	
	//����������ӵĿͻ���ip
	string Client_ip;

	//����������ӵĿͻ��˶˿�
	int Client_Port;


};