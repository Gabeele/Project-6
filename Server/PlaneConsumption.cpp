#define _CRT_SECURE_NO_WARNINGS

#include "PlaneConsumption.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

PlaneConsumption::PlaneConsumption(string PlaneId) {
	this->PlaneId = PlaneId;
	cout << "ID: " << this->PlaneId;

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
	auto end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);


	std::ostringstream oss;
	oss << this->PlaneId << "_" << this->StartTime << "_" 
		<< std::ctime(&end_time) << ".txt";

	std::string filename = oss.str();

	fstream file_out;

	file_out.open(filename, std::ios_base::out);
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