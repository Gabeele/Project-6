#pragma once

#include <string>
using namespace std;

class PlaneConsumption
{

public:
	string PlaneId;
	time_t StartTime;
	float StartWeight;
	float Average = 0;
	int DataPoints = 0;

	explicit PlaneConsumption(string PlaneId);
	void calcAverage(time_t date, float weight);
	void SaveToFile();

};