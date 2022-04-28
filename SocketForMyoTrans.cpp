#include "SocketForMyoTrans.h"

SocketForMyoTrans::SocketForMyoTrans(const char* IP, const int port)
	: IP(IP), port(port) {
}

SocketForMyoTrans::~SocketForMyoTrans() {
	//Closing socket
	if (servSocket != NULL)
		closesocket(servSocket);
	if (clntSocket != NULL)
		closesocket(clntSocket);
	//Stoping the use of DLL
	WSACleanup();
}

int SocketForMyoTrans::Initalization() {

#pragma region Initalization，use the socket of version 2,2
	//after that we can use other function in socket library
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
		return -1;
	}

	//if (LOBYTE(wsaData.wVersion) != 2 ||
	//	HIBYTE(wsaData.wVersion) != 2)
	//{
	//	WSACleanup();
	//	return -1;
	//}
#pragma endregion

#pragma region Assign port (IP is any)
	//INADDR_ANY express that any address can be sent by the port what we set
	memset(&servAddr, 0, sizeof(servAddr));  //Padding each byte with 0
	servAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	servAddr.sin_family = PF_INET;   //IPv4
	servAddr.sin_port = htons(port);
	return 0;
#pragma endregion
}

int SocketForMyoTrans::ServerConnect() {

#pragma region Run Initalization
	if (Initalization() != 0)
	{
		return -1;
	}
#pragma endregion

#pragma region Initialization servSocket and determine if -1
	servSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (servSocket == INVALID_SOCKET)
	{
		printf("servSocket failed!\n");
		return -1;
	}
	printf("servSocket success!\n");
#pragma endregion

#pragma region Seting Socket to non-blocking mode
	if (iMode == 1)
	{
		retVal = ioctlsocket(servSocket, FIONBIO, (u_long FAR*) & iMode);
		if (retVal == SOCKET_ERROR)
		{
			printf("ioctlsocket failed!\n");
			WSACleanup();
			return -1;
		}
	}
#pragma endregion

#pragma region Bind socket and address in server and determine the succession
	int b = bind(servSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (b == SOCKET_ERROR)
	{
		printf("servSocket bind failed!\n");
		return -1;
	}
	printf("servSocket bind success!\n");
#pragma endregion

#pragma region  Listen the servSocket and determine the succession
	int l = listen(servSocket, 1);
	if (l == SOCKET_ERROR)
	{
		printf("servSocket listen failed!\n");
		return -1;
	}
	printf("servSocket listening ...\n");
#pragma endregion

#pragma region  Accept clntAddr and determine the succession
	while (true)
	{
		//if connSocket do not accept client's connect , test will stop here
		clntSocket = accept(servSocket, (SOCKADDR*)&clntAddr, &clntlen);
		if (clntSocket == INVALID_SOCKET)
		{
			int err = WSAGetLastError(); //used to get the corresponding error code at last time
			if (err != 0)
			{
				Sleep(100);
				continue;
			}
		}
		break;
	}
	printf("servSocket accept success ...\n");
	return 0;
#pragma endregion
}

int SocketForMyoTrans::ClientConnect() {
#pragma region Run Initalization
	if (Initalization() != 0)
	{
		return -1;
	}
	printf("connecting……\n");
#pragma endregion

#pragma region Initialization servSocket and determine if -1
	clntSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (clntSocket == INVALID_SOCKET)
	{
		printf("clntSocket failed!\n");
		return -1;
	}
	printf("clntSocket success!\n");
#pragma endregion

#pragma region Connect clntSocket and servAddr in server and determine the succession
	while (1) {
		int b = connect(clntSocket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));
		if (b == SOCKET_ERROR)
		{
			//printf("clntSocket connect failed!\n");
			Sleep(100);
			printf("connect waiting......\n");
			continue;
		}
		break;
	}
	printf("clntSocket connect success!\n");
	return 0;
#pragma endregion
}

void SocketForMyoTrans::ServerReconnect()
{
	if (isReconnect)
	{
		printf("reconnect...\n");
		while (true)
		{
			clntSocket = accept(servSocket, (SOCKADDR*)&clntAddr, &clntlen);
			if (clntSocket == INVALID_SOCKET)
			{
				int err = WSAGetLastError();
				if (err != 0)
				{
					Sleep(100);
					continue;
				}
			}
			break;
		}
		isReconnect = false;
		printf("reconnect sucess!\n");
	}
}

void SocketForMyoTrans::ClientReconnect()
{
	if (isReconnect)
	{
		printf("reconnect...\n");
		clntSocket = socket(PF_INET, SOCK_STREAM, 0);
		if (clntSocket == INVALID_SOCKET)
		{
			printf("socket failed!\n");
			return;
		}
		while (true)
		{
			if (connect(clntSocket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
			{
				Sleep(100);
				continue;
			}
			break;
		}
		isReconnect = false;
		printf("reconnect sucess!\n");
	}
}

void SocketForMyoTrans::SendData(std::string data) {

	sprintf(buff, "%s", data.c_str());
	if (send(clntSocket, buff, Buf_Size, 0) == SOCKET_ERROR) {
		printf("Send failed!\n");
		isReconnect = true;
		return;
	}
	printf("Send Message: %s \n", buff);
}

std::string SocketForMyoTrans::RecvData() {
	memset(buff, 0, Buf_Size);
	if (recv(clntSocket, buff, Buf_Size, 0) == SOCKET_ERROR) {
		printf("Receive failed!\n");
		isReconnect = true;
		return "";
	}
	printf("Recieve Message: %s \n", buff);
	return buff;
}