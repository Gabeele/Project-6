#define _CRT_SECURE_NO_WARNINGS

#include "PlaneConsumption.h"
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;

PlaneConsumption::PlaneConsumption(string PlaneId) {
	this->PlaneId = PlaneId;
	cout << "ID: " << this->PlaneId << endl;

}

void PlaneConsumption::calcAverage(time_t date, float weight)
{
	if (DataPoints == 0)
	{
		StartTime = date;
		StartWeight = weight;
		Average = StartWeight;
	}
	else
	{
		EndTime = date;
		Average = (StartWeight - weight) / (date - StartTime);
	}

	DataPoints++;
}

void PlaneConsumption::SaveToFile()
{
	cout << "Writing to file" << endl;
	auto end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	char buffer[80];
	std::strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", std::localtime(&end_time));

	std::ostringstream oss;
	oss << this->PlaneId << "_" << buffer << ".txt";

	std::string filename = oss.str();

	std::ofstream file_out(filename);

	if (!file_out.is_open()) {
		cout << "failed to open " << filename << '\n';
	}
	else {
		file_out << "ID: " << this->PlaneId << endl;
		file_out << "Fuel consumption: " << this->Average  << "lbs/s" << endl;
		file_out << "Flight time: " << this->EndTime - this->StartTime << " s" << endl;
	}

	file_out.close();
}