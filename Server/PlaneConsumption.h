#pragma once


#include <string>
using namespace std;

class PlaneConsumption
{

public:
	string PlaneId;
	float Average = 0;
	int DataPoints = 0;

	explicit PlaneConsumption(string PlaneId);
	float calcAverage(time_t date, float data);

};