#include <windows.networking.sockets.h>
#include <iostream>
#include <thread>
#include <string>
#include "PlaneConsumption.h"
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

void UpdateData(unsigned int, float);
float CalcAvg(unsigned int);
void ReceiveData(SOCKET);

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


	while (true) {
		ConnectionSocket = SOCKET_ERROR;
		ConnectionSocket = accept(ServerSocket, NULL, NULL);
		if (ConnectionSocket == SOCKET_ERROR) {
			cerr << "Failed to accept connection." << endl;
			continue;
		}

		thread clientThread(ReceiveData, ConnectionSocket);
		clientThread.detach();
	}

	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	return 1;
}


void ReceiveData(SOCKET ConnectionSocket)
{

	char RxBuffer[30] = {}; // buffer to store incoming packet
	int bytesReceived = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0); // receive packet from client

	// Add a plane id to
	string s(RxBuffer);
	PlaneConsumption plane(s);

	while (bytesReceived > 0) {
		int bytesReceived = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);

		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0){
			cout << "Client disconnected " << endl;
			closesocket(ConnectionSocket);
			return;
		}


		string::size_type pos = s.find_first_of(',');	// Gets the date 
		string token = s.substr(0, pos);

		int epochTime = stoi(token);	// Obtains the epochtime

		chrono::system_clock::time_point tp = chrono::system_clock::time_point(chrono::seconds(epochTime));	// Converts epoch time to a date time
		time_t date = chrono::system_clock::to_time_t(tp);

		pos = s.find_first_of(',');	// Gets the data
		token = s.substr(0, pos + 1);


		float data = stof(token);

		// calcualte the average 
		plane.calcAverage(date, data);

	}

	// Close connection socket
	closesocket(ConnectionSocket);
}

 
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
