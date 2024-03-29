#include "../include/eulerianField.h"

// functions for the eulerian field class

// start empty field
eulerianField::eulerianField()
	:NumberHeights(0), NumberWidths(0), Height(0), Width(0), FieldName("")
{}

// build field with correct number of divisions
eulerianField::eulerianField(int InputNumberHeights, int InputNumberWidths, float InputHeight, float InputWidth, const char* InputFieldName)
	:NumberHeights(InputNumberHeights), NumberWidths(InputNumberWidths), Height(InputHeight), Width(InputWidth), FieldName(InputFieldName)
{
	Field = new long double[NumberHeights*NumberWidths]; // stored quantity
	numberPoints = new int[NumberHeights*NumberWidths]; // number of particles in each cell
	Heights = new float[NumberHeights];
	Widths = new float[NumberWidths];

	for (long double* initValue = Field; initValue != Field + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}

	// create list of division positions
	float HeightStep = Height/NumberHeights;
	float WidthStep = Width/NumberWidths;

	float step = 0;
	for (float* heigth = Heights; heigth != Heights+NumberHeights; ++heigth)
	{
		*heigth = step;
		step += HeightStep;
	}
	step = 0;
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
	{
		*width = step;
		step += WidthStep;
	}

	newFile();
}

// virtual destructor
eulerianField::~eulerianField()
{
	//clearMemory();
}

// delete stored informations
void eulerianField::resetField()
{
	for (long double* initValue = Field; initValue != Field + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
}

// frees memory after use
void eulerianField::clearMemory()
{
	delete[] Field;
	delete[] numberPoints;
	delete[] Heights;
	delete[] Widths;
}

// takes the average in each cell
void eulerianField::consolidateField()
{
	for (int i = 0; i < NumberHeights*NumberWidths; ++i)
	{
		if ( numberPoints[i] == 0)
		{
			Field[i] = 0;
			continue;
		}
		Field[i] = Field[i]/numberPoints[i];
	}
}

// create new file for store informations with appropriate header
void eulerianField::newFile()
{
	std::fstream file;
	file.open("Files/"+FieldName+".txt", std::ios::out | std::ios::trunc);

	file << "---Geometry---" << std::endl;
	file << "Heights:";
	for (float* height = Heights; height != Heights+NumberHeights; ++height)
		file << " " << *height;
	file << std::endl;
	
	file << "Widths:";
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
		file << " " << *width;
	file << std::endl;

	file << "---" << FieldName << "---" << std::endl;

	file.close();
}

// save the informations of current frame
void eulerianField::writeFrame(int FrameNumber)
{
	std::fstream file;
	file.open("Files/"+FieldName+".txt", std::ios::out | std::ios::app);

	file << FrameNumber << " :";
	for (int i = 0; i < NumberHeights*NumberWidths; i++)
	{
		if (numberPoints[i] == 0)
			file <<	" NaN";
		else
			file << " " << Field[i];
	}
//	for (long double* value = Field; value != Field + NumberHeights*NumberWidths; ++value)
//		file << " " << *value;
	file << std::endl;

	file.close();

	resetField();
}

// put the information of a particle in the correct cell
void eulerianField::addParticle(float fieldValue, float x, float y)
{
	int heightIndex = 0, widthIndex = 0;
	
	// iterates to find the correct cell height
	for (; heightIndex < NumberHeights; ++heightIndex)
	{
		if ( x < Heights[heightIndex])
		{
			break;
		}
	}
	heightIndex--;

	// the same for the width
	for (; widthIndex < NumberWidths; ++widthIndex)
	{
		if ( y < Widths[widthIndex])
		{
			break;
		}
	}
	widthIndex--;
	
	//std::cout << x << " , " << y << " : " << heightIndex << " , " << widthIndex << std::endl;

	// sum the data in the cell and add a particle in the cell counter
	Field[heightIndex*NumberWidths + widthIndex] = Field[heightIndex*NumberWidths + widthIndex] + fieldValue;
	numberPoints[heightIndex*NumberWidths + widthIndex]++;
	//std::cout << heightIndex << " , " << widthIndex << " : " << Field[heightIndex*NumberWidths+widthIndex] << " , " << numberPoints[heightIndex*NumberWidths+widthIndex] << std::endl;
}

// same functions but for granular temperature
eulerianFieldGranularTemp::eulerianFieldGranularTemp()
	:NumberHeights(0), NumberWidths(0), Height(0), Width(0), FieldName("")
{}

eulerianFieldGranularTemp::eulerianFieldGranularTemp(int InputNumberHeights, int InputNumberWidths, float InputHeight, float InputWidth, const char* InputFieldName)
	:NumberHeights(InputNumberHeights), NumberWidths(InputNumberWidths), Height(InputHeight), Width(InputWidth), FieldName(InputFieldName)
{
	UField = new long double[NumberHeights*NumberWidths];
	VField = new long double[NumberHeights*NumberWidths];
	Temperature = new long double[NumberHeights*NumberWidths];
	numberPoints = new int[NumberHeights*NumberWidths];
	Heights = new float[NumberHeights];
	Widths = new float[NumberWidths];

	for (long double* initValue = UField; initValue != UField + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = VField; initValue != VField + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = Temperature; initValue != Temperature + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}

	float HeightStep = Height/NumberHeights;
	float WidthStep = Width/NumberWidths;

	float step = 0;
	for (float* heigth = Heights; heigth != Heights+NumberHeights; ++heigth)
	{
		*heigth = step;
		step += HeightStep;
	}
	step = 0;
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
	{
		*width = step;
		step += WidthStep;
	}

	newFile();
}

eulerianFieldGranularTemp::~eulerianFieldGranularTemp()
{
	//clearMemory();
}

void eulerianFieldGranularTemp::resetField()
{
	for (long double* initValue = UField; initValue != UField + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = VField; initValue != VField + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = Temperature; initValue != Temperature + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberHeights*NumberWidths; initValue++)
	{
		*initValue = 0;
	}
	VelocityVector.clear();
}

void eulerianFieldGranularTemp::clearMemory()
{
	delete[] UField;
	delete[] VField;
	delete[] Temperature;
	delete[] numberPoints;
	delete[] Heights;
	delete[] Widths;
}

// only change, calculate average velocity to calculate granular temperature
void eulerianFieldGranularTemp::consolidateField()
{
	for (int i = 0; i < NumberHeights*NumberWidths; ++i)
	{
		if ( numberPoints[i] == 0)
		{
			Temperature[i] = 0;
			continue;
		}
		float AvgU = UField[i]/numberPoints[i];
		float AvgV = VField[i]/numberPoints[i];
		for (auto &Velocity : VelocityVector)
		{
			if (Velocity.index == i)
			{
				Temperature[i] = Temperature[i] + pow(Velocity.u - AvgU,2)/2.0 + pow(Velocity.v - AvgV,2)/2.0;
			}
		}
		Temperature[i] = Temperature[i] / numberPoints[i];
	}
}

void eulerianFieldGranularTemp::newFile()
{
	std::fstream file;
	file.open("Files/"+FieldName+".txt", std::ios::out | std::ios::trunc);

	file << "---Geometry---" << std::endl;
	file << "Heights:";
	for (float* height = Heights; height != Heights+NumberHeights; ++height)
		file << " " << *height;
	file << std::endl;
	
	file << "Widths:";
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
		file << " " << *width;
	file << std::endl;

	file << "---" << FieldName << "---" << std::endl;

	file.close();
}

void eulerianFieldGranularTemp::writeFrame(int FrameNumber)
{
	std::fstream file;
	file.open("Files/"+FieldName+".txt", std::ios::out | std::ios::app);

	file << FrameNumber << " :";
//	for (long double* value = Temperature; value != Temperature + NumberHeights*NumberWidths; ++value)
//		file << " " << *value;
	for (int i = 0; i < NumberHeights*NumberWidths; i++)
	{
		if ( numberPoints[i] == 0 )
			file <<	" NaN";
		else
			file << " " << Temperature[i];
	}
	file << std::endl;

	file.close();

	resetField();
}

void eulerianFieldGranularTemp::addParticle(float u, float v, float x, float y)
{
	int heightIndex = 0, widthIndex = 0;
	
	for (; heightIndex < NumberHeights; ++heightIndex)
	{
		if ( x < Heights[heightIndex])
		{
			break;
		}
	}
	heightIndex--;
	for (; widthIndex < NumberWidths; ++widthIndex)
	{
		if ( y < Widths[widthIndex])
		{
			break;
		}
	}
	widthIndex--;

	UField[heightIndex*NumberWidths + widthIndex] = UField[heightIndex*NumberWidths + widthIndex] + u;
	VField[heightIndex*NumberWidths + widthIndex] = VField[heightIndex*NumberWidths + widthIndex] + v;
	numberPoints[heightIndex*NumberWidths + widthIndex]++;
	VelocityVector.push_back(Velocity(u,v,heightIndex*NumberWidths+widthIndex));
	//std::cout << heightIndex << " , " << widthIndex << " : " << Field[heightIndex*NumberWidths+widthIndex] << " , " << numberPoints[heightIndex*NumberWidths+widthIndex] << std::endl;
}

// auxiliary function to cell positions
float eulerianField::cellTop(int fieldAdress)
{
	int height = fieldAdress / NumberWidths;
	if (height < (NumberHeights - 1))
		return Heights[height+1];

	return Heights[NumberHeights-1] + (Heights[1] - Heights[0]);
}

float eulerianField::cellBottom(int fieldAdress)
{
	int height = fieldAdress / NumberWidths;
	return Heights[height];
}

// number of cells
int eulerianField::fieldSize()
{
	return NumberHeights * NumberWidths;
}
