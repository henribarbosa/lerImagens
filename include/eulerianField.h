#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

// Structure to store quantities in cells dividing the bed

class eulerianField
{
private:
	int NumberHeights, NumberWidths; // divisions of the bed
	float Height, Width; // size of the bed
	int* numberPoints = nullptr; // divisions calculated
	float* Heights = nullptr;
	float* Widths = nullptr;
	std::string FieldName; // name for saving the variable

	void newFile(); // creates new file for saving
	void resetField(); // reset the field for next frame

public:
	long double* Field = nullptr; // store the values

	eulerianField(int NumberHeights, int NumberWidths, float Height, float Width, const char* fieldName); //create field with empty cells
	eulerianField(); // create an empty field, to declare a variable
	virtual ~eulerianField(); // virtual destructor
	void clearMemory(); // frees the memory after saving
	void addParticle(float fieldValue, float x, float y); // add a particle's quantity in the correct cell
	void consolidateField(); // calculate averages in each cell
	void writeFrame(int frameNumber); // write the current frame to saving file
	float cellTop(int fieldAdress); // top position of a cell, to compare with a particle position
	float cellBottom(int fieldAdress); // bottom position of a cell
	int fieldSize(); // number of celss
};

// structure for saving particles velocities
struct Velocity
{
	Velocity(float ui, float vi, int indexi)
		:u(ui), v(vi), index(indexi)
	{}

	float u{}, v{};
	int index{};
};

// the same as previous, but modified for granular temperature calculations
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
