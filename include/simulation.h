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

// class to process the simulation data

class Simulation_data
{
private:
	float tube_diameter = 0, tube_height = 0; // 3D parameters

	int frame = 0;
	int atomsNumber = 0; // number of particles
	float height = 0;

	float* points = nullptr; // points positions
	std::vector<std::pair<float,float>> plugsVector = {}; // plugs positions

public:
	Simulation_data();
	eulerianFieldSimulation VelocityX; // eulerian fileds
	eulerianFieldSimulation VelocityY;
	eulerianFieldSimulation VelocityZ;
	eulerianFieldSimulation ParticleNumber1;
	eulerianFieldSimulation ParticleNumber2;
	eulerianFieldGranularTempSimulation GranularTemperature;
	eulerianFieldPartFracSimulation ParticleFraction;

	void readFile(std::string file, std::string method, bool bidisperse);
	void newFile(std::string name); // new saving file
	void appendToFile(std::string fileName, std::string line); // save new frame
	void findPlugs(); // look for plugs
};

