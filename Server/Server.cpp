#include <windows.networking.sockets.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

struct StorageTypes 
{ 
	unsigned int size = 0;	// Length of the column 
	float* pData;			// The actual value
};
StorageTypes RxData[7];		// Creates 8 RxData units

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

	while (RxBuffer[0] != '*')	// The 
	{
		float fValue = 0;
		memset(RxBuffer, 0, sizeof(RxBuffer));	// Making the RxBuffer 0
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Reveices the buffer
		send(ConnectionSocket, "ACK", sizeof("ACK"), 0);	// Sends an ackknowledgment
		// Checks what the buffer's string has
		if (strcmp(RxBuffer, "ACCELERATION BODY X") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer)); // Clearing RxBuffer
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
			fValue = (float)atof(RxBuffer);	// Assigns the float value from the buffer
			UpdateData(0, fValue);	// Calls to update
			fValue = CalcAvg(0);		// Calculates the average
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
			// Missing time stamp -- will lead to the time stampe being printed at 0.0000 
			memset(RxBuffer, 0, sizeof(RxBuffer));
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = 0.0;
		}

		// Sends the average back to the client
		char Tx[128];
		sprintf_s(Tx, "%f", fValue);
		send(ConnectionSocket, Tx, sizeof(Tx), 0);
	}

	closesocket(ConnectionSocket);	//closes incoming socket
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	return 1;
}

// 
void UpdateData(unsigned int uiIndex, float value)
{
	// First time a value is entered will configure the column
	if (RxData[uiIndex].size == 0)
	{
		RxData[uiIndex].pData = new float[1];	
		RxData[uiIndex].pData[0] = value;
		RxData[uiIndex].size = 1;
	}
	else     // Any time there is more than 1 in the column, it will reconfigure the memory 
	{
		float* pNewData = new float[RxData[uiIndex].size + 1]; // re-creating the float pointer with memory space for 1 additional float
		for (unsigned int x = 0; x < RxData[uiIndex].size; x++)	// Iterate through the old values and assign the old values into the newly created float ptr array.
			pNewData[x] = RxData[uiIndex].pData[x];

		pNewData[RxData[uiIndex].size] = value; // appends the newly received float onto the end of the array
		delete[] RxData[uiIndex].pData; // free old memory
		RxData[uiIndex].pData = pNewData; // reassign the structure member variable
		RxData[uiIndex].size++; // incrememnt the structure member size counter
	}
}

// Calcuates the average in all the values from 0 to uiIndex. It is a running average
float CalcAvg(unsigned int uiIndex)
{
	float Avg = 0;
	for (unsigned int x = 0; x < RxData[uiIndex].size; x++)	// Calculates the average of the current column lenght 
		Avg += RxData[uiIndex].pData[x];

	Avg = Avg / RxData[uiIndex].size;
	return Avg;
}