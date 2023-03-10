#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

unsigned int GetSize();

int main()
{
	WSADATA wsaData;
	SOCKET ClientSocket;
	sockaddr_in SvrAddr;
	unsigned int uiSize = 0;
	vector<string> ParamNames;
	char Rx[128];

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPv4 TCP protocol 
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(27001);	// Port 27001
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//Localhost
	connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

	uiSize = GetSize();	// Obtains the number of lines in the file
	for (unsigned int l = 0; l < uiSize; l++)	// For each line in the file. l = line NOT 1!
	{
		string strInput;
		ifstream ifs("DataFile.txt");
		for (unsigned int iStart = 0; iStart < l; iStart++)	// Finds the starting line by running through the file after the previously read line
			getline(ifs, strInput);

		getline(ifs, strInput);	// Gets the current line
		if (l > 0)	// Only after the first iteration will this execute
		{
			size_t offset, preOffset;
			offset = preOffset = 0;
			unsigned int iParamIndex = 0;
			//while (offset != std::string::npos)
			while(iParamIndex != 8)	// Sets 8 iterations of the data file 
			{
				offset = strInput.find_first_of(',', preOffset+1);	// Finds the first comma and sets the offset to the index of the comma + 1 
				string strTx = strInput.substr(preOffset+1, offset - (preOffset+1));
				send(ClientSocket, ParamNames[iParamIndex].c_str(), (int)ParamNames[iParamIndex].length(), 0);		// Sends the current parameter title (What the data is)
				recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response
				send(ClientSocket, strTx.c_str(), (int)strTx.length(), 0);		// Sends the data 
				recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response
				cout << ParamNames[iParamIndex] << " Avg: " << Rx << endl;		// Prints what was recevied 
				preOffset = offset;	// Sets the pre offset to the next offset to get the next piece of data
				iParamIndex++;	// Increment the param index
			}
		}
		else // Runs the first line to get the column names
		{
			ParamNames.push_back("TIME STAMP");		// Pushes time stamp into the param names vector
			size_t offset, preOffset;	
			offset = 0;
			preOffset = -1;
			while (offset != std::string::npos)		// While it is not the end of the string
			{
				offset = strInput.find_first_of(',', preOffset + 1);	// Parse by comma
				string newParam = strInput.substr(preOffset + 1, offset - (preOffset + 1));	// Get the parameter 
				ParamNames.push_back(newParam);	// Pushes the new parameter to the param names
				preOffset = offset;
			}
		}
		ifs.close();	 // Closes the file
	}

	closesocket(ClientSocket);
	WSACleanup();

	return 1;
}

// Obtains the number of lines in the file
unsigned int GetSize()
{
	string strInput;
	unsigned int uiSize = 0;
	ifstream ifs("DataFile.txt");	// Specifically in this file
	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			getline(ifs, strInput);
			uiSize++;
		}
	}

	return uiSize;
}