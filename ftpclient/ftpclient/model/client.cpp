#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include"../Head/client.h"
#include"../Head/util.h"
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<direct.h>
#include<fstream>
#include<vector>
#pragma comment(lib,"Ws2_32.lib")

using namespace std;

bool Client::running() {

	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}

	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (-1 == sockfd) {

		printf("Create socket error(%d): %S\n", errno, strerror(errno));
		return false;

	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;

	//ip地址转化函数
	inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

	servaddr.sin_port = htons(SERVER_PORT);

	printf("connect...\n");

	if (-1 == connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
		printf("Connect error(%d):%s\n", errno, strerror(errno));
		return false;
	}

	struct hostent* host;

	host = gethostbyaddr((char*)&servaddr.sin_addr, sizeof(servaddr.sin_addr) - 1, AF_INET);

	char* ip;

	ip = inet_ntoa(*(struct in_addr*)*host->h_addr_list);

	cout <<"服务器的ip是 ：" << ip << endl;

	memset(Buff, 0, BUFFSIZE);

	int messLen = recv(sockfd, Buff, BUFFSIZE, 0);
	//得到服务器返回的connect信息
	if (messLen == -1) {
		cout << "Recv Connect Message Failed " << errno << strerror(errno) << endl;
		return false;
	}
	else if (messLen == 0) {
		cout << "Recv Connect Message Len is 0 " << endl;
		return false;
	}

	cout << "Recv Connect Message : " << Buff << endl;

	string filePath;
	cout << "please input file : ";
	cin >> filePath;

	//获取文件名
	Util ut;
	vector<string> PathVec = ut.splitString(filePath, "/");
	int VecSize = PathVec.size();
	string filename = PathVec[VecSize - 1];
	strcpy(Buff, filename.c_str());

	//发送文件名
	send(sockfd, Buff, BUFFSIZE, 0);
	
	//发送文件
	sendFile(filename);
	
}


bool Client::sendFile(string filePath) {

	//获取当前目录
	char *workPath = _getcwd(NULL, 0);

	if (workPath == NULL) {
		cout << "Get current work path failed \n";
	}
	else {
		cout << "Current work path is " << workPath << endl;
	}

	string absoPath = workPath;

	absoPath += "/";
	absoPath += filePath;

	ifstream file(absoPath.c_str(), ios::binary);

	if (!file) {
		cout << "File opened failed. \n";
		return false;
	}
	else {
		cout << "Open the file : " << absoPath << " success " << endl;
	}

	//获取文件的长度
	long fBegin = file.tellg();
	file.seekg(0, ios::end);
	long fEnd = file.tellg();

	int fileSize = fEnd - fBegin;

	cout << "File's Size is" << fileSize << "bytes" << endl;

	//发送文件的长度
	_itoa_s(fileSize, Buff, 10);
	send(sockfd, Buff, BUFFSIZE, 0);


	

	cout << "Begin to read file..." << endl;
	file.seekg(0, ios::beg);

	int sendLen = 0;
	while (true) {
		memset(Buff, 0, BUFFSIZE);
		file.read(Buff, BUFFSIZE);

		int len = send(sockfd, Buff, BUFFSIZE, 0);

		if (len == -1) {
			cout << "Send file occur error " << errno << strerror(errno);
			return false;
		}

		sendLen += BUFFSIZE;
		cout << "Send " << sendLen << "/" << fileSize << "bytes\n";

		if (sendLen >= fileSize) {
			break;
		}


	}

	file.close();
	cout << "Send file successful \n";

	return true;
}

