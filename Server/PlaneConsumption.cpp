#include "PlaneConsumption.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

PlaneConsumption::PlaneConsumption(string PlaneId) {
	this->PlaneId = PlaneId;
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
		cout << date - StartTime;
		Average = (weight - StartWeight) / (date - StartTime);
	}
}

void PlaneConsumption::SaveToFile()
{
	std::ostringstream oss;
	oss << this->PlaneId << "_" << this->StartTime << ".txt";
	std::string filename = oss.str();

	string filename(filename);
	fstream file_out;

	file_out.open(filename, std::ios_base::out);
	if (!file_out.is_open()) {
		cout << "failed to open " << filename << '\n';
	}
	else {
		file_out << "Some random text to write." << endl;
		cout << "Done Writing!" << endl;
	}
}


/// testing
	/*time_t now1 = time(NULL);
	std::this_thread::sleep_for(std::chrono::seconds(5));
	time_t now2 = time(NULL);
	cout << now2 - now1;*/