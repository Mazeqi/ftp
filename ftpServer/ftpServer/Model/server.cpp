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
#include<direct.h>
#pragma comment(lib,"Ws2_32.lib")

//void server(SOCKET s);
Server::Server(SOCKET s):server(s) 
{
	storePath = "./";
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
			cout << "The client ip " << inet_ntoa(sa.sin_addr) << " client PORT " << ntohs(sa.sin_port)  <<" has closed. " << endl << endl;
			return false;
		}
		else {
			if (recv(server, Buff, BUFFSIZE, 0) == -1) {
				cout << "recv command error" << endl;
				return false;
			}
			
			Util ut;

			//得到command
			string command = string(Buff);

			cout << "Receive the client ip " << Client_ip << " client Port " << Client_Port << "'s command : " << command << endl;
			cout << "The Result : ";

			vector<string> strVec;

			//进行解析
			CMD cmd = commandParse(strVec, command);

			switch (cmd) {
				case ERR: {
					cout << "Command is invalid \n";

					memset(Buff, 0, BUFFSIZE);
					string err = "Command is invalid \n";
					strcpy(Buff, err.c_str());

					if (send(server, Buff, BUFFSIZE, 0) == -1) {
						cout << "Send Cmd is invalid Message occur error \n";
						return false;
					}

					break;
				}

				case USER: {
					User(strVec);
					break;
				}

				case PASS: {
					Pass(strVec);
					break;
				}
			   
				case DIR: {
					fileDir();
					break;
				}

				case PUT: {
					sendFile(strVec);
					break;
				}

				case GET: {
					recvFile(strVec);
					break;
				}

			}

		}
	}
	
	return true;
}

bool Server::sendFile(vector<string> strVec) {

	if (confirmStatus() == false) {
		return false;
	}

	//获取当前目录
	//此信息回送给sendcommand
	char* workPath = _getcwd(NULL, 0);

	if (workPath == NULL) {
		string err = "Get  work path failed .";
		string sendErr = "Send message of getting work path occur error.";
		sendERR(err, 0, sendErr, 1);
		return false;
	}
	else {
		string err = "Get file path success";
		string sendErr = "Send message of file path success occur error.";
		sendERR(err, 1, sendErr, 1);
		cout << "Current work path is " << workPath << endl;
	}

	//文件
	string filePath = strVec[1];

	//文件目录
	string absoPath = workPath;

	absoPath += "/";
	absoPath += filePath;

	ifstream file(absoPath.c_str(), ios::binary);

	//发送文件打开的信息
	if (!file) {
		string err = "Open the file  failed.";
		string sendErr = "Send the message of open the file occur error";
		sendERR(err, 0, sendErr, 1);
		cout << "Open the file : " << absoPath << " failed " << endl;
		return false;
	}
	else {
		string err = "Open the file  success.";
		string sendErr = "Send the message of open the file occur error";
		sendERR(err, 1, sendErr, 1);
		cout << "Open the file : " << absoPath << " success " << endl;
		
	}

	//获取文件的长度
	long fBegin = file.tellg();
	file.seekg(0, ios::end);
	long fEnd = file.tellg();
	int fileSize = fEnd - fBegin;

	
	string err = "File's Size is " + to_string(fileSize);
	err += " bytes";
	string sendErr = "Send message of file size occur error";
	cout << err << endl;

	//发送文件的长度
	memset(Buff, 0, BUFFSIZE);
	_itoa_s(fileSize, Buff, 10);
	if (send(server, Buff, BUFFSIZE, 0) == -1) {
		cout << sendErr << endl;
		return false;
	}

	cout << "Begin to read file..." << endl;
	file.seekg(0, ios::beg);

	int sendLen = 0;
	while (true) {
		memset(Buff, 0, BUFFSIZE);
		file.read(Buff, BUFFSIZE);

		int len = send(server, Buff, BUFFSIZE, 0);

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

	//回送关于指令信息
	if (USER_Status == 0) {
		
		string err = "Please confirm the username before other cmd.";
		string sendErr = "Send PassWord Error Message occur error.";
		sendERR(err, 0, sendErr, 1);

		return;
	}

	if (PASS_Status == 1) {
		string err = "Have confirmed  the username and password.";
		string sendErr = "Send confirmed user and pass Error Message occur error ";
		sendERR(err, 0, sendErr, 1);
		
		return;
	}

	//会送指令处理信息
	string PassWord = strVec[1];
	if (PassWord == "1234") {
		PASS_Status = 1;
		string err = "The PassWord is OK. ";
		string sendErr = "Send PassWord OK Message occur error. ";
		sendERR(err, 1, sendErr, 1);

	}
	else {
		PASS_Status = 0;
		string err = "The PassWord is ERROR. ";
		string sendErr = "Send PassWord Error Message occur error. ";
		sendERR(err, 0, sendErr, 1);
	}
}

void Server::User(vector<string> strVec) {
	if (USER_Status == 1) {
		string err = "Have confirmed the username.";
		string sendErr =  "Send confirm user Error Message occur error."; 
		sendERR(err, 0, sendErr, 1);
		return;
	}

	string UserName = strVec[1];
	if (UserName == "Mazeqi") {
		USER_Status = 1;

		//发送错误信息
		string err = "The UserName is OK.";
		string sendErr = "Send UserName Error Message occur error.";
		sendERR(err, 1, sendErr, 1);

	}
	else {
		
		string err = "The UserName is ERROR";
		string sendErr = "Send UserName Error Message occur error.";
		sendERR(err, 0, sendErr, 1);
	}
}

void Server::sendERR(string err, int errStatus, string sendErr, int sendErrStatus) {

	memset(Buff, 0, BUFFSIZE);
	strcpy(Buff, err.c_str());
	cout << err << endl;

	if (send(server, Buff, BUFFSIZE, 0) == -1) {
		cout << sendErr << endl;
		return;
	}

	if (sendErrStatus) {
		_itoa_s(errStatus, Buff, 10);
		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << sendErr << endl;
			return;
		}
	}
	
}

void Server::fileDir() {

	//回送指令信息
	if (confirmStatus() == false) {
		return;
	}

	Util ut;
	vector<string> fileVec = ut.dirFile("./");
	int vecSize = fileVec.size();
	
	//先发送列表长度信息
	string err = "The size of file is : " + to_string(vecSize);
	err += "\n";
	string sendErr = "Send the err of the file size occur error. ";
	sendERR(err, 1, sendErr, 1);


	//发送长度
	memset(Buff, 0, BUFFSIZE);
	_itoa_s(vecSize, Buff, 10);
	if (send(server, Buff, BUFFSIZE, 0) == -1) {
		cout << "Send the size of the file occur error \n";
		return;
	}

	//发送列表
	for (int i = 0; i < vecSize; i++) {

		memset(Buff, 0, BUFFSIZE);
		strcpy(Buff, fileVec[i].c_str());
		cout << Buff << endl;

		if (send(server, Buff, BUFFSIZE, 0) == -1) {
			cout << "Send the err of the file name occur error \n";
			return;
		}

	}
	cout << endl;
}

bool Server::confirmStatus() {

	if (USER_Status == 0) {
		
		string err = "Have not confirm the username. ";
		string sendErr = "Send the message of having not confirmed the username occur error. ";
		sendERR(err, 0, sendErr, 1);
		return false;
	}

	if (PASS_Status == 0) {

		string err = "Have not confirm the password. \n";
		string sendErr = "Send the message of having not confirmed the password occur error ";
		sendERR(err, 0, sendErr, 1);
		return false;

	}
	return true;
}

CMD Server::commandParse(vector<string> &strVec,string command) {
	Util ut;

	strVec = ut.splitString(command, " ");

	transform(strVec[0].begin(), strVec[0].end(), strVec[0].begin(), ::tolower);

	int vecSize = strVec.size();

	if (vecSize != 2) {
		if (vecSize == 1 && (strVec[0] == "dir")){
			return DIR;
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

	//client发来get指令，则服务器要put一个文件到client
	if (strVec[0] == "get") {
		return PUT;
	}

	if (strVec[0] == "put") {
		return GET;
	}
	return ERR;


}

bool Server::recvFile(vector<string> strVec) {

	if (confirmStatus() == false) {
		return false;
	}

	//回应command指令
	memset(Buff, 0, BUFFSIZE);
	string err = "Server is ok !";
	string sendErr = "Send Server ok occur error";
	sendERR(err, 1, sendErr, 1);


	
	//接收文件大小的信息
	memset(Buff, 0, BUFFSIZE);

	recv(server, Buff, BUFFSIZE, 0);

	int fileSize = atoi(Buff);

	cout << "File's size is " << fileSize << endl;

	string filename = strVec[1];
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
	
	//接收完成信息
	/*memset(Buff, 0, BUFFSIZE);
	string err = "Server receive file successfully !";
	string sendErr = "Send Server  file successfully  occur error";
	sendERR(err, 0, sendErr, 0);*/

	return true;

}