#include "../Head/server.h"
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<fstream>
#include<errno.h>
#include<string>
#include<algorithm>
#include"../Head/Util.h"
#pragma comment(lib,"Ws2_32.lib")

//void server(SOCKET s);
Server::Server(SOCKET s):server(s) 
{
	storePath = "./file/";
	USER_Status = 0;
	PASS_Status = 0;
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

	//现实客户ip 
	struct sockaddr_in sa;
	int len = sizeof(sa);
	if (!getpeername(server, (struct sockaddr*)&sa, &len))
	{
		Client_ip = string(inet_ntoa(sa.sin_addr));
		Client_Port = ntohs(sa.sin_port);
		cout << "The client ip " << Client_ip << " client PORT " << Client_Port << " has linked. " << endl;
		
	}
	
	while (true) {
		
		if (!checkSock(server)) {
			cout << "The client ip " << inet_ntoa(sa.sin_addr) << "client PORT " << ntohs(sa.sin_port)  <<" has closed. " << endl << endl;
			return false;
		}
		else {
			if (recv(server, Buff, BUFFSIZE, 0) == -1) {
				cout << "recv command error" << endl;
				return false;
			}
			
			cout << "Receive the client ip " << Client_ip << " client Port " << Client_Port << "'s command ."  << endl;
			cout << "The Result : ";
			
			Util ut;

			//得到command
			string command = string(Buff);

			vector<string> strVec;

			//进行解析
			CMD cmd = commandParse(strVec, command);

			switch (cmd) {

				case USER: {
					User(strVec);
					break;
				}
				case PASS: {
					Pass(strVec);
					break;
				}
			}

		}
	}
	
	return true;
}

bool Server::checkSock(int sock) {
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

void Server::Pass(vector<string> strVec) {

	if (USER_Status == 0) {
		memset(Buff, 0, BUFFSIZE);
		string err = "Please confirm the username before other cmd.\n";
		strcpy(Buff, err.c_str());
		cout << err << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout  << "Send PassWord Error Message occur error. \n";
			return;
		}
		return;
	}

	if (PASS_Status == 1) {
		memset(Buff, 0, BUFFSIZE);
		string err = "Have confirmed  the username and password.\n";
		strcpy(Buff, err.c_str());
		cout << err << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send confirmed user and pass Error Message occur error \n";
			return;
		}
		return;
	}

	string PassWord = strVec[1];
	if (PassWord == "1234") {
		PASS_Status = 1;

		memset(Buff, 0, BUFFSIZE);
		string err = "The PassWord is OK\n";
		strcpy(Buff, err.c_str());

		cout << err << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send PassWord OK Message occur error \n";
			return;
		}
	}
	else {

		memset(Buff, 0, BUFFSIZE);
		string err = "The PassWord is ERROR\n";
		strcpy(Buff, err.c_str());
		cout << err << endl;
		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send PassWord Error Message occur error \n";
			return;
		}
	}
}

void Server::User(vector<string> strVec) {
	if (USER_Status == 1) {

		memset(Buff, 0, BUFFSIZE);
		string err = "Have confirmed the username \n";
		strcpy(Buff, err.c_str());
		cout << err << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send confirm user Error Message occur error. \n";
			return;
		}
		return;
	}

	string UserName = strVec[1];
	if (UserName == "Mazeqi") {

		USER_Status = 1;
		string err = "The UserName is OK\n";
		strcpy(Buff, err.c_str());
		cout << err << endl;
		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send UserName Error Message occur error \n";
			return;
		}
	}
	else {
		memset(Buff, 0, BUFFSIZE);
		string err = "The UserName is ERROR\n";
		strcpy(Buff, err.c_str());
		cout << err << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send UserName Error Message occur error \n";
			return;
		}
	}
}

CMD Server::commandParse(vector<string> &strVec,string command) {
	Util ut;

	strVec = ut.splitString(command, " ");

	if (strVec.size() != 2) {
		return ERR;
	}


	transform(strVec[0].begin(), strVec[0].end(), strVec[0].begin(), ::tolower);


	if (strVec[0] == "user") {
		return USER;
	}

	if (strVec[0] == "pass") {
		return PASS;
	}

	if (strVec[0] == "dir") {
		return DIR;
	}

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