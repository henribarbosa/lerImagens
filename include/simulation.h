#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

#include "read_control.h"
#include "eulerianFieldSimulation.h"

class Simulation_data
{
private:
	float tube_diameter = 0, tube_height = 0;

	int frame = 0;
	int atomsNumber = 0;
	float height = 0;

	float* points = nullptr;
	std::vector<std::pair<float,float>> plugsVector = {};

public:
	Simulation_data();
	eulerianFieldSimulation VelocityX;
	eulerianFieldSimulation VelocityY;
	eulerianFieldSimulation VelocityZ;
	eulerianFieldSimulation ParticleNumber1;
	eulerianFieldSimulation ParticleNumber2;
	eulerianFieldGranularTempSimulation GranularTemperature;
	eulerianFieldPartFracSimulation ParticleFraction;

	void readFile(std::string file, std::string method, bool bidisperse);
	void newFile(std::string name);
	void appendToFile(std::string fileName, std::string line);
	void findPlugs();
};

