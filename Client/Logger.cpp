#include "Logger.h"

#define _CRT_SECURE_NO_WARNINGS

Logger::Logger() {

	this->fileName = "Unset";
	this->fileSize = NULL;

}

Logger::Logger(string fileName) {

	auto time = std::chrono::system_clock::now();
	std::time_t endtime = std::chrono::system_clock::to_time_t(time);

	cout << std::ctime(&endtime);

	this->fileName = fileName + "_" + (std::ctime(&endtime)) + ".txt";

	fileStream.open(this->fileLocation + "/" + this->fileName);
	if (!fileStream) {
		cout << "Failed to open log file.";
	}

}

void Logger::PrintToLogFile(string toBePrinted) {

	this->fileStream << toBePrinted << endl;

	return;

}