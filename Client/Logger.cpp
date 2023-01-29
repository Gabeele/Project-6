#include "Logger.h"

#define _CRT_SECURE_NO_WARNINGS

Logger::Logger() {

	this->logFileName = "Unset";
	this->logFileSize = NULL;

}

Logger::Logger(string directoryName) {

	char* dirName = new char[directoryName.length() + 1];
	strcpy(dirName, directoryName.c_str());
	//8 extra bytes for "./Logs//0", 1 more the dirName null terminator.
	char* pathName = new char[directoryName.length() + 9];
	char logsArray[8] = "./Logs/";
	strcpy(pathName, logsArray);
	strcat(pathName, dirName);

	this->logFileLocation = "./Logs/" + directoryName;

	int checkLogsDir = _mkdir(logsArray);
	// check if directory is created or not
	if (!checkLogsDir)
		printf("Directory created\n");
	else {
		printf("Unable to create directory\n");
	}

	int checkLoggerFileDir = _mkdir(pathName);

	// check if directory is created or not
	if (!checkLoggerFileDir)
		printf("Directory created\n");
	else {
		printf("Unable to create directory\n");
	}

	//auto time = std::chrono::system_clock::now();
	std::time_t endtime = std::time(0);
	std::tm* now = std::localtime(&endtime);

	this->logFileName = directoryName + '_' + to_string(1900 + now->tm_year) + '-' + to_string(1 + now->tm_mon) + '-' + to_string(now->tm_mday)
		+ '-' + to_string(now->tm_hour) + '-' + to_string(now->tm_min) + '-' + to_string(now->tm_sec) + ".txt";
	
	string filePath = this->logFileLocation + "/" + this->logFileName;

	logFileStream.open(filePath);
	if (!logFileStream) {
		cout << "Failed to open log file.";
	}

}

void Logger::PrintToLogFile(string toBePrinted) {

	this->logFileStream << toBePrinted << endl;

	return;

}

void Logger::PrintToLogFile(string toBePrinted, chrono::system_clock::time_point startTime, chrono::system_clock::time_point endTime) {

	std::chrono::duration<double> elapsedTime = endTime - startTime;

	string strDuration = to_string(elapsedTime.count());

	this->logFileStream << toBePrinted << " " << strDuration << endl;

	return;

}

void Logger::PrintToLogFile(string toBePrinted, int metricToPrint) {

	this->logFileStream << toBePrinted << " " << metricToPrint << endl;

	return;

}

void Logger::PrintToLogFile(string toBePrinted, std::chrono::duration<double> metricToPrint) {

	this->logFileStream << toBePrinted << " " << to_string(metricToPrint.count()) << endl;

	return;

}