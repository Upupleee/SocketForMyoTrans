#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <stdio.h>
//#include <Winsock2.h>
#include <windows.h>
#include <iostream>

//After add qcustomplot.h and cpp , winsock2 return redefine
//So add windows.h instead of winsock2.h  //winsock can also useful

#pragma comment(lib,"WS2_32.lib")

#define Buf_Size 1024

typedef struct MyoBuf
{
	float x;
	float y;
	float z;
};

//server will just set port to receive data
//but client need both IP and port to send data

class SocketForMyoTrans {

public:
	SocketForMyoTrans(const char* IP, const int port);
	~SocketForMyoTrans();

	//Function
	int ServerConnect();  // bind/listen/accept
	int ClientConnect(); // connect
	//ServerConnect——RecvData and RecvImg
	//ServerConnect——SendData and SendImg
	void ServerReconnect();
	void ClientReconnect();
	void setMode(int val) { iMode = val; };
	void SendData(std::string data);
	std::string RecvData();

	//Value
	SOCKET clntSocket;
	//clntSocket:
	//1、in ServerConnect() , it use to receive data in RecvData
	//2、in ClientConnect() , it use to send data and image in SendData

private:
	int Initalization();
	WSADATA wsaData;

	const char* IP = "127.0.0.1";
	int port = 6668;

	//socket and address
	SOCKET servSocket; //the socket in local

	sockaddr_in servAddr;
	sockaddr_in clntAddr;
	int clntlen = sizeof(sockaddr);
	int retVal;
	int iMode;
	bool isReconnect = false;

	char buff[Buf_Size];  //To receive data
	int needRecv;  //To get sizeof(RecvBuf)
	int count; //To judge if the picture data receive completely
};