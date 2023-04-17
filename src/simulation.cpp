#include "../include/simulation.h"

Simulation_data::Simulation_data()
//	:frame(0),height(0),points(nullptr)
{
	newFile("Expansion");
//	newFile("Plugs");
//	newFile("Particle fraction");

	control diamControl("thresholds.txt","SimulationDiameter");
	tube_diameter = diamControl.returnThreshold();
	control heightControl("thresholds.txt","SimulationHeight");
	tube_height = heightControl.returnThreshold();
	VelocityX = eulerianFieldSimulation(50,3,tube_height,tube_diameter,"VelocityX");
	VelocityY = eulerianFieldSimulation(50,3,tube_height,tube_diameter,"VelocityY");
	VelocityZ = eulerianFieldSimulation(50,3,tube_height,tube_diameter,"VelocityZ");
	ParticleNumber1 = eulerianFieldSimulation(50,3,tube_height,tube_diameter,"Particles1");
	ParticleNumber2 = eulerianFieldSimulation(50,3,tube_height,tube_diameter,"Particles2");
	GranularTemperature = eulerianFieldGranularTempSimulation(30,3,tube_height,tube_diameter,"GranularTemperature");
	ParticleFraction = eulerianFieldPartFracSimulation(50,1,tube_height,tube_diameter,"PartcleFraction");
}

void Simulation_data::readFile(std::string filePath, std::string method, bool bidisperse)
{
	height = 0;

	int label, type, density;
	float x, y, z, u1, u2, u3, f1, f2, f3, fd1, fd2, fd3, radius;

	std::ifstream file;
	std::string line;

	file.open(filePath);

	std::getline(file, line); //time step label
	std::getline(file, line); // time step
	frame = std::stoi(line);

	std::getline(file, line); // number atoms label
	std::getline(file, line); // number atoms
	atomsNumber = std::stoi(line);

//	points = new float[4*atomsNumber];

	std::getline(file, line); // bounding box label
	std::getline(file, line); // ... x
	std::getline(file, line); // ... y
	std::getline(file, line); // ... z

	std::getline(file, line); // atoms label;
	for (int i = 0; i < atomsNumber; i++)
	{
		file >> label >> type >> x >> y >> z >> u1 >> u2 >> u3 >> f1 >> f2 >> f3 
			>> fd1 >> fd2 >> fd3 >> radius >> density;

//		std::cout << z << std::endl;

//		points[4*i] = x;
//		points[4*i+1] = y;
//		points[4*i+2] = z;
//		points[4*i+3] = radius;

		if (z + radius > height)
			height = z + radius;

		if (method == "expansion")
			continue;

		VelocityX.addParticle(u1,x,y,z);
		VelocityY.addParticle(u2,x,y,z);
		VelocityZ.addParticle(u3,x,y,z);
		GranularTemperature.addParticle(u1,u2,u3,x,y,z);
		ParticleFraction.addParticleFraction(radius,x,y,z);

		if (method == "temperature")
			continue;

		if (type == 1)
		{
			ParticleNumber1.addParticle(1,x,y,z);
		}
		else {
			ParticleNumber2.addParticle(1,x,y,z);
		}

	}

	file.close();

//	findPlugs();

	std::stringstream writeLine;

	writeLine << frame << " : " << height;
	appendToFile("Expansion", writeLine.str());
	writeLine.clear();

//	delete[] points;

	if (method == "expansion")
		return;

	VelocityX.consolidateField();
	VelocityX.writeFrame(frame);
	VelocityY.consolidateField();
	VelocityY.writeFrame(frame);
	VelocityZ.consolidateField();
	VelocityZ.writeFrame(frame);
	ParticleNumber1.writeFrame(frame);
	ParticleNumber2.writeFrame(frame);
	GranularTemperature.consolidateField();
	GranularTemperature.writeFrame(frame);
}

void Simulation_data::newFile(std::string name)
{
	std::ofstream file;
	file.open("PostFiles/"+name+".txt", std::ios::trunc);
	file << "";
	file.close();
}

void Simulation_data::appendToFile(std::string fileName, std::string line)
{
	std::ofstream file;
	file.open("PostFiles/"+fileName+".txt", std::ios::app);
	file << line << std::endl;
	file.close();
}

void Simulation_data::findPlugs()
{
	plugsVector.clear();
	std::stringstream writeLine;
	writeLine << frame << " : ";

	control plug_threshold("thresholds.txt","SimulationPlugThreshold");
	float threshold = plug_threshold.returnThreshold();

	bool inPlug = false;
	std::pair<float,float> currentPlug;

	for (float h = 0; h < height; h += 0.0001)
	{
		float sum_area = 0;
		for (int i = 0; i < atomsNumber; i++)
		{
//			std::cout << points[4*i] << " , " << points[4*i+1] << " , " << points[4*i+2] << " , " << points[4*i+3] << " , " << h - points[4*i+2]/2 << std::endl;
			if (points[4*i+2] - h > points[4*i+3]/2)
				continue;
			if (points[4*i+2] - h < -points[4*i+3]/2)
				continue;
//			std::cout << "OK: " << points[4*i] << " , " << points[4*i+1] << " , " << points[4*i+2] << " , " << points[4*i+3] << " , " << h - points[4*i+2] << std::endl;
			float area = M_PI * ( pow(points[4*i+3]/2,2) - pow((h - points[4*i+2]),2) );
			sum_area += area;
		}
		sum_area = sum_area / (M_PI * pow(tube_diameter/2,2));
		writeLine << sum_area << " ";
//		std::cout << h << " : " << sum_area << std::endl;
		if (sum_area >= threshold)
		{
			if (not inPlug)
			{
				currentPlug.first = h;
				inPlug = true;
			}
		}
		else 
		{
			if (inPlug)
			{
				currentPlug.second = h;
				plugsVector.push_back(currentPlug);
				inPlug = false;
			}
		}
	}
	appendToFile("Particle fraction", writeLine.str());

	writeLine.clear();
	writeLine << frame << " : ";
	for (auto eachPlug : plugsVector)
	{
		writeLine << eachPlug.first << " " << eachPlug.second << " ";
	}
	appendToFile("Plugs", writeLine.str());

}


