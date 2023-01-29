#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Logger.h"
#include "LoggingConfiguration.h"

using namespace std;

unsigned int GetSize();

// Performance Metrics 
int NumbeOfPacketsSent = 0;
int NumbeOfPacketsRecv = 0;
int TotalPacketSizeSent = 0;

//File IO metrics.
int NumTimesFilesOpen = 0;
int NumberOfBytesReadFromFile = 0;
int byteSizeOfLargestLineRead = 0; //Bytesize of line reads captured to give context to the line read times in future iterations of the code base.
	//std::chrono::duration<double> totalCurrentLineReadTime; //Time it takes for the intended line to be read.
	//std::chrono::duration<double> totalLineIndexingTime; //Time it takes for the program to 'get it's bearings' and arrive at the correct line.
std::chrono::duration<double> totalLineReadTime; // Sum of above two metrics.
	//std::chrono::duration<double> maxCurrentLineReadTime;
	//std::chrono::duration<double> maxLineIndexingTime;
std::chrono::duration<double> maxLineReadTime;
	//std::chrono::duration<double> avgCurrentLineReadTime;
	//std::chrono::duration<double> avgLineIndexingTime;
std::chrono::duration<double> avgLineReadTime;

chrono::time_point<chrono::system_clock> startTime, endTime;
chrono::time_point<chrono::system_clock> startTime2, endTime2;

Logger fileIOLogger = Logger("FileIO");
Logger DataTransmissionLogger = Logger("DataTransmission");
//Logger fileIOLogger = Logger("FileIO");
//Logger fileIOLogger = Logger("FileIO");
//Logger fileIOLogger = Logger("FileIO");

int main()
{
	//Check conditions to determine which performance testing configuration we are currently executing in.
#if FILEIOTESTING == true
	cout << "Testing File IO\n";
#endif

#if OTHERTESTING == true
	cout << "Other Testing\n";
#endif

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

	//Opens at line 59, ends at line 68. Captures the time it takes to get the file size by the GetSize() function as well as the number of bytes read from the file, then logs the results.
#if FILEIOTESTING == true
	startTime = chrono::system_clock::now();
#endif

	uiSize = GetSize();	// Obtains the number of lines in the file

#if FILEIOTESTING == true
	endTime = chrono::system_clock::now();
	fileIOLogger.PrintToLogFile("Total seconds taken by GetSize() function call:", startTime, endTime);
	fileIOLogger.PrintToLogFile("Number of Bytes read from file:", NumberOfBytesReadFromFile);
#endif

	for (unsigned int l = 0; l < uiSize; l++)	// For each line in the file. l = line NOT 1!
	{
		string strInput;
		ifstream ifs("DataFile.txt");

		//Opens at line 77, ends at line 172. Increments the file opened counter each iteration of the loop. Logs the number of times the file was opened.
		//Starts the clock to measure the amount of time it takes to read the current line in one iteration of the loop.
#if FILEIOTESTING == true
		NumTimesFilesOpen++;
		startTime = chrono::system_clock::now();
#endif
		for (unsigned int iStart = 0; iStart < l; iStart++) {	// Finds the starting line by running through the file after the previously read line
			getline(ifs, strInput);
		}

#if FILEIOTESTING == true
		startTime2 = chrono::system_clock::now();
#endif
		getline(ifs, strInput);
#if FILEIOTESTING == true
		endTime2 = chrono::system_clock::now();
		endTime = chrono::system_clock::now();
#endif

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

#if DATATRANSMISSION == true
				startTime = chrono::system_clock::now();
#endif

				send(ClientSocket, ParamNames[iParamIndex].c_str(), (int)ParamNames[iParamIndex].length(), 0);		// Sends the current parameter title (What the data is)

#if DATATRANSMISSION == true
				endTime = chrono::system_clock::now();
				DataTransmissionLogger.PrintToLogFile("Time to send packet (Client)", startTime, endTime);

				TotalPacketSizeSent += sizeof(ParamNames[iParamIndex].c_str());
				NumbeOfPacketsSent++;
#endif
				
#if DATATRANSMISSION == true
				startTime = chrono::system_clock::now();
#endif

				recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response

#if DATATRANSMISSION == true
				endTime = chrono::system_clock::now();
				DataTransmissionLogger.PrintToLogFile("Time to recv packet (Client)", startTime, endTime);

				NumbeOfPacketsRecv++;
#endif

#if DATATRANSMISSION == true
				startTime = chrono::system_clock::now();
#endif
				send(ClientSocket, strTx.c_str(), (int)strTx.length(), 0);		// Sends the data 
#if DATATRANSMISSION == true
				endTime = chrono::system_clock::now();
				DataTransmissionLogger.PrintToLogFile("Time to send packet (Client)", startTime, endTime);

				TotalPacketSizeSent += sizeof(ParamNames[iParamIndex].c_str());
				NumbeOfPacketsSent++;
#endif

#if DATATRANSMISSION == true
				startTime = chrono::system_clock::now();
#endif
				recv(ClientSocket, Rx, sizeof(Rx), 0);		// Waits for response
#if DATATRANSMISSION == true
				endTime = chrono::system_clock::now();
				DataTransmissionLogger.PrintToLogFile("Time to recv packet (Client)", startTime, endTime);

				NumbeOfPacketsRecv++;
#endif

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


		//Avg time of line read
		//avg size of line read
		//longest line read
		//long time to read a line
#if FILEIOTESTING == true
		std::chrono::duration<double> elapsedTime = endTime - startTime;
		totalLineReadTime += (elapsedTime);
		if (elapsedTime > maxLineReadTime) {
			maxLineReadTime = elapsedTime;
		}
		avgLineReadTime = totalLineReadTime / NumTimesFilesOpen;
#endif



	}

#if FILEIOTESTING == true
	fileIOLogger.PrintToLogFile("totalLineReadTime:", totalLineReadTime);
	fileIOLogger.PrintToLogFile("maxLineReadTime:", maxLineReadTime);
	fileIOLogger.PrintToLogFile("avgLineReadTime:", avgLineReadTime);
	fileIOLogger.PrintToLogFile("Number of times data file was open:", NumTimesFilesOpen);
#endif

#if DATATRANSMISSION == true
	DataTransmissionLogger.PrintToLogFile("Total Number of packets sent (Client)", NumbeOfPacketsSent);
	DataTransmissionLogger.PrintToLogFile("Total Number of packets recv (Client)", NumbeOfPacketsRecv);
	DataTransmissionLogger.PrintToLogFile("Total packet byte size sent (Client)", TotalPacketSizeSent);
#endif

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

			int currentLineBytesRead = strInput.length() * sizeof(char);
			NumberOfBytesReadFromFile += currentLineBytesRead;
			if (byteSizeOfLargestLineRead < currentLineBytesRead) {
				byteSizeOfLargestLineRead = currentLineBytesRead;
			}

			uiSize++;
		}
	}

	return uiSize;
}