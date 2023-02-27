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

		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
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