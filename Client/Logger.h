#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>

using namespace std;

class Logger {

	public:

		//Public class members

		ofstream fileStream;
		string fileName;
		string fileLocation = "./Logs";
		int fileSize;

		//Constructor
		Logger();
		Logger(string);
	
		//Public utility methods
		void PrintToLogFile(string);

};
