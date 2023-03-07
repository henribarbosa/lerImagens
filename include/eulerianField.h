#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

class eulerianField
{
private:
	int NumberHeights, NumberWidths;
	float Height, Width;
	int* numberPoints = nullptr;
	long double* Field = nullptr;
	float* Heights = nullptr;
	float* Widths = nullptr;
	std::string FieldName;

	void newFile();
	void resetField();

public:
	eulerianField(int NumberHeights, int NumberWidths, float Height, float Width, const char* fieldName);
	eulerianField();
	virtual ~eulerianField();
	void clearMemory();
	void addParticle(float fieldValue, float x, float y);
	void consolidateField();
	void writeFrame(int frameNumber);
};

struct Velocity
{
	Velocity(float ui, float vi, int indexi)
		:u(ui), v(vi), index(indexi)
	{}

	float u{}, v{};
	int index{};
};

class eulerianFieldGranularTemp
{
private:
	int NumberHeights, NumberWidths;
	float Height, Width;
	int* numberPoints = nullptr;
	long double* UField = nullptr;
	long double* VField = nullptr;
	long double* Temperature = nullptr;
	float* Heights = nullptr;
	float* Widths = nullptr;
	std::string FieldName;
	std::vector<Velocity> VelocityVector;

	void newFile();
	void resetField();
	void clearMemory();

public:
	eulerianFieldGranularTemp(int NumberHeights, int NumberWidths, float Height, float Width, const char* fieldName);
	eulerianFieldGranularTemp();
	virtual ~eulerianFieldGranularTemp();
	void addParticle(float u, float v, float x, float y);
	void consolidateField();
	void writeFrame(int frameNumber);
};
