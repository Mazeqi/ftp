#include "../Head/server.h"
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<fstream>
#include<errno.h>
#pragma comment(lib,"Ws2_32.lib")

//void server(SOCKET s);
Server::Server(SOCKET s):server(s) 
{
	storePath = "./temp/";
}

void Server::WelCome() {
	char welcome[] = "WelCome to Connect \n";

	int isend = send(server, welcome, 1024, 0);
	if (isend == -1) {
		cout << "Send WelCome Error " << errno << strerror(errno) << endl;
		return;
	}
}

bool Server::running() {

	WelCome();

	if (recv(server, Buff, BUFFSIZE, 0) == 0) {
		cout << "recv() Failed! \n";
		return false;
	}
	else {
		//接收文件名
		//memset(Buff, 0, BUFFSIZE);

		//recv(server, Buff, BUFFSIZE, 0);

		string filename = string(Buff);

		cout << "File's name is " << filename << endl;

		RecvFile(filename);
	}
	return true;
	
}

bool Server::RecvFile(string filename) {

	
	//接收文件大小的信息
	memset(Buff, 0, BUFFSIZE);

	recv(server, Buff, BUFFSIZE, 0);

	int fileSize = atoi(Buff);

	cout << "File's size is " << fileSize << endl;

	//打开文件
	string filepath = storePath + filename;

	ofstream storeFile(filepath, ios::binary);

	if (!storeFile) {
		cout << "Create File Failed!\n";
		return false;
	}


	//接收文件内容，以binary形式接收
	int fileLen = fileSize;
	int recvLen = 0;
	while (true) {
		memset(Buff, 0, sizeof(Buff));
		int len = recv(server, Buff, BUFFSIZE, 0);
		if (len == -1) {
			cout << "recv error" << errno << strerror(errno);
			return false;
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
	cout << "Write file successful\n";

	return true;

}