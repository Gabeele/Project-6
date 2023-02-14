#include <windows.networking.sockets.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include "../Client/LoggingConfiguration.h"
#include "../Client/Logger.h"

using namespace std;

#define MAX_DATAPOINTS_SIZE 20

struct StorageTypes
{
	unsigned int avgValue = 0;
	float dataPoints[20] = { 0 };
	float* bufferPtr = &(dataPoints[0]);
};
StorageTypes RxData[7];

void UpdateData(unsigned int, float);
float CalcAvg(unsigned int);

// Preformance metrics 
int NumbeOfPacketsSent = 0;
int NumbeOfPacketsRecv = 0;
int TotalPacketSizeRecv = 0;
int TotalPacketSizeSend = 0;
int updateDataCalls = 0;
int calculateAverageCalls = 0;
int ParsingCounter = 0;
int ComaprisonCounter = 0;
chrono::time_point<chrono::system_clock> startTime, endTime;
chrono::time_point<chrono::system_clock> startTime2, endTime2;
chrono::time_point<chrono::system_clock> startTime3, endTime3;

// Logging objects
Logger DataTransmissionLogger = Logger("DataTransmission-Server");
Logger MemoryManagementLogger = Logger("MemoryManagement-Server");
Logger ParsingLogger = Logger("Parsing-Server");
Logger CalculationLogger = Logger("Calculation-Server");

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

#if DATATRANSMISSION == true
		startTime = chrono::system_clock::now();
#endif
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Reveices the buffer

#if DATATRANSMISSION == true
		endTime = chrono::system_clock::now();
		DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
		DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

		TotalPacketSizeRecv += sizeof(RxBuffer);
		NumbeOfPacketsRecv++;
#endif


#if DATATRANSMISSION == true
		startTime = chrono::system_clock::now();
#endif
		send(ConnectionSocket, "ACK", sizeof("ACK"), 0);	// Sends an ackknowledgment
#if DATATRANSMISSION == true
		endTime = chrono::system_clock::now();
		DataTransmissionLogger.PrintToLogFile("Time to send packet (Server)", startTime, endTime);
		DataTransmissionLogger.PrintToLogFile("Send packet size (Server)", sizeof("ACK"));

		TotalPacketSizeSend += sizeof("ACK");
		NumbeOfPacketsSent++;
#endif


#if PARSING == true
		startTime2 = chrono::system_clock::now();
		ParsingCounter++;
#endif
		// Checks what the buffer's string has
		if (strcmp(RxBuffer, "ACCELERATION BODY X") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 1;
			startTime3 = chrono::system_clock::now();
#endif
			memset(RxBuffer, 0, sizeof(RxBuffer)); // Clearing RxBuffer

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);	// Assigns the float value from the buffer

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(0, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif

#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(0);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif

#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ACCELERATION BODY X", startTime3, endTime3);
#endif

		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Y") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 2;
			startTime3 = chrono::system_clock::now();
#endif

			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(1, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif

#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(1);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif


#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ACCELERATION BODY Y", startTime3, endTime3);
#endif
		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Z") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 3;
			startTime3 = chrono::system_clock::now();
#endif

			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(2, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif

#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(2);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif

#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ACCELERATION BODY Z", startTime3, endTime3);
#endif

		}
		else if (strcmp(RxBuffer, "TOTAL WEIGHT") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 4;
			startTime3 = chrono::system_clock::now();
#endif

			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif
			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(3, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif

#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(3);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif

#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for TOTAL WEIGHT", startTime3, endTime3);
#endif
		}
		else if (strcmp(RxBuffer, "PLANE ALTITUDE") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 5;
			startTime3 = chrono::system_clock::now();
#endif
			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(4, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif


#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(4);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif


#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for PLANE ALTITUDE", startTime3, endTime3);
#endif

		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR PITCH DEGREES") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 6;
			startTime3 = chrono::system_clock::now();
#endif

			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(5, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif


#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(5);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif

#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ATTITUDE INDICATOR PICTH DEGREES", startTime3, endTime3);
#endif
		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR BANK DEGREES") == 0)
		{
#if PARSING == true
			ComaprisonCounter += 7;
			startTime3 = chrono::system_clock::now();
#endif

			memset(RxBuffer, 0, sizeof(RxBuffer));
#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = (float)atof(RxBuffer);

#if MEMORYMANAGEMENT == true
			startTime = chrono::system_clock::now();
#endif
			UpdateData(6, fValue);
#if MEMORYMANAGEMENT == true
			endTime = chrono::system_clock::now();
			MemoryManagementLogger.PrintToLogFile("Time for memory management call", startTime, endTime);
#endif

#if CALCULATIONS == true
			startTime = chrono::system_clock::now();
#endif
			fValue = CalcAvg(6);		// Calculates the average
#if CALCULATIONS == true
			endTime = chrono::system_clock::now();
			CalculationLogger.PrintToLogFile("Time for CalcAvg call", startTime, endTime);
#endif


#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ATTITUDE INDICATOR BANK DEGREES", startTime3, endTime3);
#endif
		}
		else
		{
#if PARSING == true
			ComaprisonCounter += 8;
			startTime3 = chrono::system_clock::now();
#endif

			// Missing time stamp -- will lead to the time stampe being printed at 0.0000 
			memset(RxBuffer, 0, sizeof(RxBuffer));

#if DATATRANSMISSION == true
			startTime = chrono::system_clock::now();
#endif
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);		// Receives the next piece of data
#if DATATRANSMISSION == true
			endTime = chrono::system_clock::now();
			DataTransmissionLogger.PrintToLogFile("Time to recv packet (Server)", startTime, endTime);
			DataTransmissionLogger.PrintToLogFile("Recv packet size (Server)", sizeof(RxBuffer));

			TotalPacketSizeRecv += sizeof(RxBuffer);
			NumbeOfPacketsRecv++;
#endif

			fValue = 0.0;

#if PARSING == true
			endTime3 = chrono::system_clock::now();
			ParsingLogger.PrintToLogFile("Comparison Time for ELSE", startTime3, endTime3);
#endif
		}
#if PARSING == true
		endTime2 = chrono::system_clock::now();
		ParsingLogger.PrintToLogFile("Total parsing time with comparison for one iteration", startTime2, endTime2);
#endif

		// Sends the average back to the client
		char Tx[128];
		sprintf_s(Tx, "%f", fValue);

#if DATATRANSMISSION == true
		startTime = chrono::system_clock::now();
#endif
		send(ConnectionSocket, Tx, sizeof(Tx), 0);
#if DATATRANSMISSION == true
		endTime = chrono::system_clock::now();
		DataTransmissionLogger.PrintToLogFile("Time to send packet (Server)", startTime, endTime);
		DataTransmissionLogger.PrintToLogFile("Send packet size (Server)", sizeof(Tx));

		TotalPacketSizeSend += sizeof(Tx);
		NumbeOfPacketsSent++;
#endif
	}

#if PARSING == true
	ParsingLogger.PrintToLogFile("The amount of times a packet was parsed", ParsingCounter);
	ParsingLogger.PrintToLogFile("The amount of times a comparison was made", ComaprisonCounter);
#endif

#if DATATRANSMISSION == true
	DataTransmissionLogger.PrintToLogFile("Total Number of packets sent (Server)", NumbeOfPacketsSent);
	DataTransmissionLogger.PrintToLogFile("Total Number of packets recv (Server)", NumbeOfPacketsRecv);
	DataTransmissionLogger.PrintToLogFile("Total packet byte size sent (Server)", TotalPacketSizeSend);
	DataTransmissionLogger.PrintToLogFile("Total packet byte size Recv (Server)", TotalPacketSizeRecv);
#endif

#if MEMORYMANAGEMENT == true
	MemoryManagementLogger.PrintToLogFile("Total number of memory management calls (Server)", updateDataCalls);
#endif

#if CALCULATIONS == true
	CalculationLogger.PrintToLogFile("Total number of CalcAvg calls made (Server)", calculateAverageCalls);
#endif

	closesocket(ConnectionSocket);	//closes incoming socket
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	return 1;
}

void UpdateData(unsigned int uiIndex, float value)
{
#if MEMORYMANAGEMENT == true
	updateDataCalls++;
#endif

	//Use uiIndex to reference the correct StoreageType object. Use dereference operator to assign the float value to the index of dataPoints that the bufferPtr points to.
	*(RxData[uiIndex].bufferPtr) = value;
	//Increment the bufferPtr after assignment. If the ptr is ever at the end of the buffer, reset bufferPtr to the beginning of the buffer.
	if (RxData[uiIndex].bufferPtr == &(RxData[uiIndex].dataPoints[sizeof(RxData[uiIndex].dataPoints)])) {
		RxData[uiIndex].bufferPtr = &(RxData[uiIndex].dataPoints[0]);
	}
	else {
		RxData[uiIndex].bufferPtr++;
	}

	//After each point calculate the average.

}


float CalcAvg(unsigned int uiIndex)
{
#if CALCULATIONS == true
	calculateAverageCalls++;
#endif

	float Avg = 0;
	for (unsigned int x = 0; x < RxData[uiIndex].size; x++)
		Avg += RxData[uiIndex].pData[x];

	Avg = Avg / RxData[uiIndex].size;
	return Avg;
}