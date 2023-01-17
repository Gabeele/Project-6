#include <windows.networking.sockets.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

struct StorageTypes 
{ 
	unsigned int size = 0;
	float* pData;
};
StorageTypes RxData[7];

void UpdateData(unsigned int, float);
float CalcAvg(unsigned int);

int main()
{
	WSADATA wsaData;
	SOCKET ServerSocket, ConnectionSocket;
	char RxBuffer[128] = {};
	sockaddr_in SvrAddr;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == SOCKET_ERROR)
		return -1;

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27001);
	bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

	if (ServerSocket == SOCKET_ERROR)
		return -1;

	listen(ServerSocket, 1);
	cout << "Waiting for client connection\n" << endl;
	ConnectionSocket = SOCKET_ERROR;
	ConnectionSocket = accept(ServerSocket, NULL, NULL);

	if (ConnectionSocket == SOCKET_ERROR)
		return -1;

	cout << "Connection Established" << endl;

	while (RxBuffer[0] != '*')
	{
		float fValue = 0;
		memset(RxBuffer, 0, sizeof(RxBuffer));
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		send(ConnectionSocket, "ACK", sizeof("ACK"), 0);
		if (strcmp(RxBuffer, "ACCELERATION BODY X") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(0, fValue);
			fValue = CalcAvg(0);
		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Y") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(1, fValue);
			fValue = CalcAvg(1);
		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Z") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(2, fValue);
			fValue = CalcAvg(2);
		}
		else if (strcmp(RxBuffer, "TOTAL WEIGHT") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(3, fValue);
			fValue = CalcAvg(3);
		}
		else if (strcmp(RxBuffer, "PLANE ALTITUDE") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(4, fValue);
			fValue = CalcAvg(4);
		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR PICTH DEGREES") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(5, fValue);
			fValue = CalcAvg(5);
		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR BANK DEGREES") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(6, fValue);
			fValue = CalcAvg(6);
		}
		else
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = 0.0;
		}

		char Tx[128];
		sprintf_s(Tx, "%f", fValue);
		send(ConnectionSocket, Tx, sizeof(Tx), 0);
	}

	closesocket(ConnectionSocket);	//closes incoming socket
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	return 1;
}

void UpdateData(unsigned int uiIndex, float value)
{
	if (RxData[uiIndex].size == 0)
	{
		RxData[uiIndex].pData = new float[1];
		RxData[uiIndex].pData[0] = value;
		RxData[uiIndex].size = 1;
	}
	else
	{
		float* pNewData = new float[RxData[uiIndex].size + 1];
		for (unsigned int x = 0; x < RxData[uiIndex].size; x++)
			pNewData[x] = RxData[uiIndex].pData[x];

		pNewData[RxData[uiIndex].size] = value;
		delete[] RxData[uiIndex].pData;
		RxData[uiIndex].pData = pNewData;
		RxData[uiIndex].size++;
	}
}

float CalcAvg(unsigned int uiIndex)
{
	float Avg = 0;
	for (unsigned int x = 0; x < RxData[uiIndex].size; x++)
		Avg += RxData[uiIndex].pData[x];

	Avg = Avg / RxData[uiIndex].size;
	return Avg;
}