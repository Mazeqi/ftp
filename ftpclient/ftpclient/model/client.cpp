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
#include<algorithm>
#include<string>
#pragma comment(lib,"Ws2_32.lib")

using namespace std;
Client::Client() {
	storePath = "./";
}
Client::~Client() {
	closesocket(sockfd);
	WSACleanup();
}

void Client::menu() {
	

	while (true) {

		char buf[1024];
		memset(buf, 0, 1024);
		fflush(stdin);

		cout << ">> ";
		cin.getline(buf, 1024);

		string command = string(buf);

		vector<string> strVec;
		CMD cmd = commandParse(strVec,command);

		
		switch (cmd) {

			case ERR:
				cout << "Command is invalid \n";
				break;

			case USER:
				sendCommand(command);
				break;

			case PASS:
				sendCommand(command);
				break;

			case CliDIR: {
				cliDir();
				break;
			}

			case ServerDIR: {
				if (sendCommand(command) == true) {
					serverDir();
				}

				break;
			}

			case GET: {
				if (sendCommand(command) == true) {
					Get(strVec);
				}
				break;
			}

			case PUT: {
				if (sendCommand(command) == true) {
					Put(strVec);
				}
				break;
			}

			case EXIT: {
				exit(0);
				break;
			}
		}
	}
}

bool Client::checkSock(int sock) {
	fd_set   fds;
	char buf[2];
	int nbread;

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	if (select(sock + 1, &fds, (fd_set*)0, (fd_set*)0, NULL) == -1) {
		//log(LOG_ERR,"select(): %s\n",strerror(errno)) ;
		return false;
	}
	if (!FD_ISSET(sock, &fds)) {
		//log(LOG_ERR,"select() returns OK but FD_ISSET not\n") ;
		return false;
	}
	/* read one byte from socket */
	nbread = recv(sock, buf, 1, MSG_PEEK);
	if (nbread <= 0) {
		return false;
	}

	return true;
}

void Client::Put(vector<string> strVec) {
	char* workPath = _getcwd(NULL, 0);

	if (workPath == NULL) {
		string err = "Get  work path failed .";
		cout << err << endl;
	
		return;
	}
	else {
		cout << "Current work path is " << workPath << endl;
	}

	//文件
	string filename = strVec[1];

	//文件目录
	string absoPath = workPath;

	absoPath += "/";
	absoPath += filename;

	ifstream file(absoPath.c_str(), ios::binary);

	//文件打开的信息
	if (!file) {
		cout << "Open the file : " << filename << " failed " << endl;
	}
	else {
		cout << "Open the file : " << filename << " success " << endl;

	}

	//获取文件的长度
	long fBegin = file.tellg();
	file.seekg(0, ios::end);
	long fEnd = file.tellg();
	int fileSize = fEnd - fBegin;


	string err = "File's Size is " + to_string(fileSize);
	err += " bytes";
	
	cout << err << endl;

	//发送文件的长度
	memset(Buff, 0, BUFFSIZE);
	_itoa_s(fileSize, Buff, 10);
	if (send(sockfd, Buff, BUFFSIZE, 0) == -1) {
		string sendErr = "Send message of file size occur error";
		cout << sendErr << endl;
		return;
	}

	cout << "Begin to read file..." << endl;
	file.seekg(0, ios::beg);

	int sendLen = 0;
	while (true) {
		memset(Buff, 0, BUFFSIZE);
		file.read(Buff, BUFFSIZE);

		int len = send(sockfd, Buff, BUFFSIZE, 0);

		if (len == -1) {
			cout << "Send file occur error " << errno << strerror(errno);
			return;
		}

		sendLen += BUFFSIZE;
		cout << "Send " << sendLen << "/" << fileSize << "bytes\n";

		if (sendLen >= fileSize) {
			break;
		}
	}

	file.close();
	cout << "Send file successful \n";
}

void Client::Get(vector <string> strVec) {

	string filename = strVec[1];

	//接受打开文件的信息
	//接收返回的指令对错信息
	memset(Buff, 0, BUFFSIZE);
	if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Recv the Return of open file occur Error " << errno << strerror(errno) << endl;
		return ;
	}
	string recvBuff = string(Buff);
	cout << recvBuff << endl;

	if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Recv the Return of open file  status occur Error " << errno << strerror(errno) << endl;
		return;
	}

	int status = atoi(Buff);
	if (status == 0) {
		return;
	}

	//接收文件大小的信息
	memset(Buff, 0, BUFFSIZE);

	recv(sockfd, Buff, BUFFSIZE, 0);

	int fileSize = atoi(Buff);

	cout << "File's size is " << fileSize << endl;

	//打开文件
	string filepath = storePath + filename;

	ofstream storeFile(filepath, ios::binary);

	if (!storeFile) {
		cout << "Create File Failed!\n";
		return;
	}


	//接收文件内容，以binary形式接收
	int fileLen = fileSize;
	int recvLen = 0;
	while (true) {
		memset(Buff, 0, sizeof(Buff));
		int len = recv(sockfd, Buff, BUFFSIZE, 0);
		if (len == -1) {
			cout << "recv error" << errno << strerror(errno);
			return;
		}

		storeFile.write(Buff, BUFFSIZE);
		recvLen += BUFFSIZE;

		cout << "Receive " << recvLen << "/" << fileSize << "  bytes\n";

		fileLen -= BUFFSIZE;
		if (fileLen <= 0) {
			break;
		}
	}

	storeFile.close();
	cout << "Write file successfully\n";
}



void Client::serverDir() {

	//接收长度信息
	memset(Buff, 0, BUFFSIZE);
	if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Recv the size of file occur Error";
		return;
	}
	int vecSize = atoi(Buff);

	//输出文件列表
	for (int i = 0; i < vecSize; i++) {
		//接收长度信息
		memset(Buff, 0, BUFFSIZE);
		if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
			cout << "Recv the  file name occur Error";
			return;
		}
		cout << Buff << endl;
	}

}

void Client::cliDir() {
	Util ut;
	vector<string> dirVec;
	dirVec = ut.dirFile("./");
	if (dirVec.size() == 0) {
		cout << "Have no file. \n";
		return;
	}
	for (int i = 0; i < dirVec.size(); i++) {
		cout << dirVec[i] << endl;
	}
}

bool Client::sendCommand(string command) {

	memset(Buff, 0, BUFFSIZE);
	strcpy(Buff, command.c_str());

	if (send(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Send User Command Error \n";
		return false;
	}
   

	//接收返回的指令对错信息
	memset(Buff, 0, BUFFSIZE);
	if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Recv the Return of User Command Error " << errno << strerror(errno) << endl;
		return false;
	}
	string recvBuff = string(Buff);
	cout << recvBuff << endl;

	if (recv(sockfd, Buff, BUFFSIZE, 0) == -1) {
		cout << "Recv the Return of User Command status Error " << errno << strerror(errno) << endl;
		return false;
	}

	int status = atoi(Buff);
	if (status == 1) {
		return true;
	}
	else {
		return false;
	}
}

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
	
	menu();

}

CMD Client::commandParse(vector<string> &strVec,string command) {

	Util ut;

	strVec = ut.splitString(command, " ");

	int vecSize = strVec.size();

	if (vecSize == 0) {
		return SPACE;
	}

	transform(strVec[0].begin(), strVec[0].end(), strVec[0].begin(), ::tolower);

	if (vecSize != 2) {
		if (vecSize == 1) {

			if (strVec[0] == "!dir") {
				return CliDIR;
			}

			if (strVec[0] == "dir") {
				return ServerDIR;
			}

			if (strVec[0] == "exit") {
				return EXIT;
			}
			return ERR;
		}
		else {
			return ERR;
		}
	
	}

	

	if (strVec[0] == "user") {
		return USER;
	}

	if (strVec[0] == "pass") {
		return PASS;
	}

	if (strVec[0] == "get") {
		return GET;
	}

	if (strVec[0] == "put") {
		return PUT;
	}
	return ERR;
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

	cout << "File's Size is " << fileSize << "bytes" << endl;

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

