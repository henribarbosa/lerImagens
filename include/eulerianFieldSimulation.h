#pragma once

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <vector>
#include <cmath>

class eulerianFieldSimulation
{
public:
	int NumberHeights, NumberWidths, NumberCells;
	float Height, Width;
	int* numberPoints = nullptr;
	float* Heights = nullptr;
	float* Widths = nullptr;
	std::string FieldName;

	void newFile();
	void resetField();

	long double* Field = nullptr;

	eulerianFieldSimulation(int NumberHeights, int NumberWidths, float Height, float Width, const char* fieldName);
	eulerianFieldSimulation();
	virtual ~eulerianFieldSimulation();
	void clearMemory();
	void addParticle(float fieldValue, float x, float y, float z);
	void consolidateField();
	void writeFrame(int frameNumber);
	float cellTop(int fieldAdress);
	float cellBottom(int fieldAdress);
	int fieldSize();
};

struct Velocity3
{
	Velocity3(float ui, float vi, float wi, int indexi)
		:u(ui), v(vi), w(wi), index(indexi)
	{}

	float u{}, v{}, w{};
	int index{};
};

class eulerianFieldGranularTempSimulation
{
private:
	int NumberHeights, NumberWidths, NumberCells;
	float Height, Width;
	int* numberPoints = nullptr;
	long double* UField = nullptr;
	long double* VField = nullptr;
	long double* WField = nullptr;
	long double* Temperature = nullptr;
	float* Heights = nullptr;
	float* Widths = nullptr;
	std::string FieldName;
	std::vector<Velocity3> VelocityVector;

	void newFile();
	void resetField();
	void clearMemory();

public:
	eulerianFieldGranularTempSimulation(int NumberHeights, int NumberWidths, float Height, float Width, const char* fieldName);
	eulerianFieldGranularTempSimulation();
	virtual ~eulerianFieldGranularTempSimulation();
	void addParticle(float u, float v, float w, float x, float y, float z);
	void consolidateField();
	void writeFrame(int frameNumber);
};

class eulerianFieldPartFracSimulation : public eulerianFieldSimulation
{
public:
	eulerianFieldPartFracSimulation()
	: eulerianFieldSimulation() { };
	eulerianFieldPartFracSimulation(int NumberHeightsI, int NumberWidthsI, float HeightI, float WidthI, const char* fieldNameI)
	: eulerianFieldSimulation(NumberHeightsI, NumberWidthsI, HeightI, WidthI, fieldNameI) { };
	void addParticleFraction(float r, float x, float y, float z);
};
