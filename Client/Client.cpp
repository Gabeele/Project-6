#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int main()
{
	WSADATA wsaData;
	SOCKET ClientSocket;
	sockaddr_in SvrAddr;
	char Rx[128];

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPv4 TCP protocol 
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(27001);	// Port 27001
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//Localhost
	connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
	
    ifstream dataFile("DataFile2.txt");

    // Check if the file is open and readable
    if (!dataFile.is_open()) {
        cout << "Could not open file." << endl;
        return 1;
    }

    // Get first line from file and extract plane id
    string PlaneID;
    getline(dataFile, PlaneID);
    string::size_type pos = PlaneID.find_first_of(': ');	// Gets the date 
    string token = PlaneID.substr(0, pos + 1);

    // Send the plane ID
    if (send(ClientSocket, PlaneID.c_str(), PlaneID.length(), 0) == SOCKET_ERROR) {
        cerr << "Error sending plane ID to server: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // consumer header line
    string line;
    getline(dataFile, line);

    // send data rows to server
    while (getline(dataFile, line)) {
        if (send(ClientSocket, line.c_str(), line.length(), 0) == SOCKET_ERROR) {
            cerr << "Error sending data row to server: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }

    dataFile.close();
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}