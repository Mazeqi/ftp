#include"../Head/Util.h"
#include<iostream>
#include<cstdio>
#include<vector>
#include"../Head/server.h"
#include<signal.h>
#include<thread>
#include<vector>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

int sockfd;
int connfd;
vector <thread*> th_manage;

void stopServerRunning(int p)
{
	closesocket(sockfd);
	WSACleanup();

	for (int i = 0; i < th_manage.size(); i++) {
		delete(th_manage[i]);
	}

	printf("Close Server\n");
	exit(0);
}

int main() {

	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}


	//用于存放ip和端口的结构
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//判断是否出错
	if (-1 == sockfd) {
		printf("Create socket error(%d): %s\n", errno, strerror(errno));
		return -1;
	}


	//将数据仓清零
	memset(&servaddr, 0, sizeof(servaddr));

	//设置ip版本
	servaddr.sin_family = AF_INET;

	//指定ip地址位通配版本
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//设置端口
	servaddr.sin_port = htons(DEFAULT_PORT);

	//第二个参数进行了参数类型转化，转化为ipv4
	if (-1 == bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
		printf("Bind error(%d): %s\n", errno, strerror(errno));
		return -1;
	}

	if (-1 == listen(sockfd, MAXLINK))
	{
		printf("Listen error(%d): %s\n", errno, strerror(errno));
		return -1;
	}

	printf("Listening...\n");

	while (true) {
		// 这句用于在输入Ctrl+C的时候关闭服务器
		signal(SIGINT, stopServerRunning);

		connfd = accept(sockfd, NULL, NULL);

		if (-1 == connfd)
		{
			printf("Accept error(%d): %s\n", errno, strerror(errno));
			return -1;
		}

		/*memset(buff, 0, BUFFSIZE);

		recv(connfd, buff, BUFFSIZE - 1, 0);

		printf("Recv: %s\n", buff);

		send(connfd, buff, strlen(buff), 0);

		closesocket(connfd);*/

		Server *server = new Server(connfd);

		thread* th = new thread(&Server::running, server);

		th_manage.push_back(th);

		th->detach();
	}

	//system("pause");
	return 0;
}