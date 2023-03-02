#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
using namespace std;

int DateToEpoch(string);

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
	
    ifstream dataFile("Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt");

    // Check if the file is open and readable
    if (!dataFile.is_open()) {
        cout << "Could not open file." << endl;
        return 1;
    }

    // Get first line from file and extract plane id
    string PlaneID;
    getline(dataFile, PlaneID);
    string::size_type pos = PlaneID.find_first_of(': ');	// Gets the date 
    string token = PlaneID.substr(pos + 1, PlaneID.length());

    // Send the plane ID
    if (send(ClientSocket, token.c_str(), token.length(), 0) == SOCKET_ERROR) {
        cerr << "Error sending plane ID to server: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response
    

    // consumer header line
    string line;
    getline(dataFile, line);


    // send data rows to server
    while (getline(dataFile, line)) {

        // get time in seconds since epoch
        string::size_type pos = line.find_first_of(',');	// Gets the date 
        string token = line.substr(0, pos);
        int epoch = DateToEpoch(token);

        std::ostringstream oss;
        oss << epoch << line.substr(pos, line.length());
        string SendLine = oss.str();

        if (send(ClientSocket, SendLine.c_str(), SendLine.length(), 0) == SOCKET_ERROR) {
            cerr << "Error sending data row to server: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cout << "Sent Data" << endl;
    } 

    cout << "Completed" << endl;

    dataFile.close();

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}

int DateToEpoch(string date)
{
    // Create a string stream from the input string
    std::istringstream iss(date);

    // Parse the date and time components
    int month, day, year, hour, min, sec;
    char delimiter;
    iss >> month >> delimiter >> day >> delimiter >> year >> hour >> delimiter >> min >> delimiter >> sec;

    // Create a std::tm struct with the parsed components
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    // Convert the std::tm to a std::chrono::system_clock::time_point
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    // Convert the time_point to epoch time
    auto epochTime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

    return (int)epochTime;
}