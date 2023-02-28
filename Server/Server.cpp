#include <windows.networking.sockets.h>
#include <iostream>
#include <thread>
#include <string>
#include "PlaneConsumption.h"
#include <chrono>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

void ReceiveData(SOCKET);

int main()
{

	WSADATA wsaData;
	SOCKET ListenSocket, ClientSocket;
	char RxBuffer[128] = {};
	sockaddr_in SvrAddr;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == SOCKET_ERROR)
		return -1;

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27001);
	bind(ListenSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

	if (ListenSocket == SOCKET_ERROR)
		return -1;

	listen(ListenSocket, 5);

	while (true) {
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			cerr << "accept() failed with error: " << WSAGetLastError() << endl;
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		cout << "Accepted a client connection" << endl;

		//ReceiveData(ClientSocket);

		thread clientThread(ReceiveData, ClientSocket);
		clientThread.detach();
	}

	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}


void ReceiveData(SOCKET ConnectionSocket)
{

	std::cout << "Recving data on thread " << std::this_thread::get_id() << std::endl;

	char RxBuffer[30] = {}; // buffer to store incoming packet
	int bytesReceived = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0); // receive packet from client
	send(ConnectionSocket, "ACK", sizeof("ACK"), 0);	// Sends an ackknowledgment
	std::cout << std::this_thread::get_id() << " Sent data" << endl;

	// Add a plane id to
	string s(RxBuffer);
	PlaneConsumption plane(s);

	while (true) {
		memset(RxBuffer, '\0', sizeof(RxBuffer));

		int bytesReceived = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) 
			break;
		std::cout << std::this_thread::get_id()  << " Recived data" << endl;
	
		send(ConnectionSocket, "ACK", sizeof("ACK"), 0);	// Sends an ackknowledgment
		std::cout << std::this_thread::get_id() << " Sent data" << endl;

		string s2(RxBuffer);
		string::size_type pos = s2.find_first_of(',');	// Gets the date 
		string token = s2.substr(0, pos);

		int epochTime = stoi(token);	// Obtains the epochtime
		chrono::system_clock::time_point tp = chrono::system_clock::time_point(chrono::seconds(epochTime));	// Converts epoch time to a date time
		time_t date = chrono::system_clock::to_time_t(tp);

		token = s2.substr(pos+1, s2.length()); // get the weight
		float data = stof(token);

		// calcualte the average 
		plane.calcAverage(date, data);

	}

	std::cout << std::this_thread::get_id()<<  " Client disconnected " << endl;

	closesocket(ConnectionSocket);

	plane.SaveToFile();

	return;
}