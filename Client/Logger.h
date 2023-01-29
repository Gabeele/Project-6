#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <direct.h>
#include <cstring>

using namespace std;

class Logger {

	public:

		//Public class members

		ofstream logFileStream;
		string logFileName;
		string logFileLocation;
		int logFileSize;

		//Constructor
		Logger();
		Logger(string);
	
		//Public utility methods
		void PrintToLogFile(string);
		void PrintToLogFile(string, chrono::system_clock::time_point, chrono::system_clock::time_point);
		void PrintToLogFile(string, int);
		void PrintToLogFile(string, std::chrono::duration<double>);

};
