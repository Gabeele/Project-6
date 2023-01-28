#include "Logger.h"

#define _CRT_SECURE_NO_WARNINGS

Logger::Logger() {

	this->logFileName = "Unset";
	this->logFileSize = NULL;

}

Logger::Logger(string directoryName) {

	this->logFileLocation = "./Logs/" + directoryName;

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