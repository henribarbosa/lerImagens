#include "../include/eulerianFieldSimulation.h"

// function for 3D eulerian field, see eulerianField.cpp for more details

eulerianFieldSimulation::eulerianFieldSimulation()
	:NumberHeights(0), NumberWidths(0), Height(0), Width(0), FieldName("")
{}

eulerianFieldSimulation::eulerianFieldSimulation(int InputNumberHeights, int InputNumberWidths, float InputHeight, float InputWidth, const char* InputFieldName)
	:NumberHeights(InputNumberHeights), NumberWidths(InputNumberWidths), Height(InputHeight), Width(InputWidth), FieldName(InputFieldName)
{
	NumberCells = NumberHeights*NumberWidths*NumberWidths;

	Field = new long double[NumberCells];
	numberPoints = new int[NumberCells];
	Heights = new float[NumberHeights];
	Widths = new float[NumberWidths];

	for (long double* initValue = Field; initValue != Field + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberCells; initValue++)
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
	step = -Width/2.0;
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
	{
		*width = step;
		step += WidthStep;
	}

	newFile();
}

eulerianFieldSimulation::~eulerianFieldSimulation()
{
	//clearMemory();
}

void eulerianFieldSimulation::resetField()
{
	for (long double* initValue = Field; initValue != Field + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberCells; initValue++)
	{
		*initValue = 0;
	}
}

void eulerianFieldSimulation::clearMemory()
{
	delete[] Field;
	delete[] numberPoints;
	delete[] Heights;
	delete[] Widths;
}

void eulerianFieldSimulation::consolidateField()
{
	for (int i = 0; i < NumberCells; ++i)
	{
		if ( numberPoints[i] == 0)
		{
			Field[i] = 0;
			continue;
		}
		Field[i] = Field[i]/numberPoints[i];
	}
}

void eulerianFieldSimulation::newFile()
{
	std::fstream file;
	file.open("PostFiles/"+FieldName+".txt", std::ios::out | std::ios::trunc);

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

void eulerianFieldSimulation::writeFrame(int FrameNumber)
{
	std::fstream file;
	file.open("PostFiles/"+FieldName+".txt", std::ios::out | std::ios::app);

	file << FrameNumber << " :";
	for (int i = 0; i < NumberCells; i++)
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

void eulerianFieldSimulation::addParticle(float fieldValue, float x, float y, float z)
{
	int heightIndex = 0, widthIndexX = 0, widthIndexY = 0;
	
	for (; heightIndex < NumberHeights; ++heightIndex)
	{
		if ( z < Heights[heightIndex])
		{
			break;
		}
	}
	heightIndex--;
	for (; widthIndexX < NumberWidths; ++widthIndexX)
	{
		if ( x < Widths[widthIndexX])
		{
			break;
		}
	}
	widthIndexX--;
	for (; widthIndexY < NumberWidths; ++widthIndexY)
	{
		if ( y < Widths[widthIndexY])
		{
			break;
		}
	}
	widthIndexY--;
	
	//std::cout << x << " , " << y << " : " << heightIndex << " , " << widthIndex << std::endl;

	Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + fieldValue;
	numberPoints[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX]++;
	//std::cout << heightIndex << " , " << widthIndex << " : " << Field[heightIndex*NumberWidths+widthIndex] << " , " << numberPoints[heightIndex*NumberWidths+widthIndex] << std::endl;
}

eulerianFieldGranularTempSimulation::eulerianFieldGranularTempSimulation()
	:NumberHeights(0), NumberWidths(0), Height(0), Width(0), FieldName("")
{}

eulerianFieldGranularTempSimulation::eulerianFieldGranularTempSimulation(int InputNumberHeights, int InputNumberWidths, float InputHeight, float InputWidth, const char* InputFieldName)
	:NumberHeights(InputNumberHeights), NumberWidths(InputNumberWidths), Height(InputHeight), Width(InputWidth), FieldName(InputFieldName)
{
//	std::cout << Width << " , " << Height << std::endl;
	NumberCells = NumberHeights*NumberWidths*NumberWidths;
	UField = new long double[NumberCells];
	VField = new long double[NumberCells];
	WField = new long double[NumberCells];
	Temperature = new long double[NumberCells];
	numberPoints = new int[NumberCells];
	Heights = new float[NumberHeights];
	Widths = new float[NumberWidths];

	for (long double* initValue = UField; initValue != UField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = VField; initValue != VField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = WField; initValue != WField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = Temperature; initValue != Temperature + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberCells; initValue++)
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
	step = -Width/2.0;
	for (float* width = Widths; width != Widths+NumberWidths; ++width)
	{
		*width = step;
		step += WidthStep;
	}

	newFile();
}

eulerianFieldGranularTempSimulation::~eulerianFieldGranularTempSimulation()
{
	//clearMemory();
}

void eulerianFieldGranularTempSimulation::resetField()
{
	for (long double* initValue = UField; initValue != UField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = VField; initValue != VField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = WField; initValue != WField + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (long double* initValue = Temperature; initValue != Temperature + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	for (int* initValue = numberPoints; initValue != numberPoints + NumberCells; initValue++)
	{
		*initValue = 0;
	}
	VelocityVector.clear();
}

void eulerianFieldGranularTempSimulation::clearMemory()
{
	delete[] UField;
	delete[] VField;
	delete[] WField;
	delete[] Temperature;
	delete[] numberPoints;
	delete[] Heights;
	delete[] Widths;
}

void eulerianFieldGranularTempSimulation::consolidateField()
{
	for (int i = 0; i < NumberCells; ++i)
	{
		if ( numberPoints[i] == 0)
		{
			Temperature[i] = 0;
			continue;
		}
		float AvgU = UField[i]/numberPoints[i];
		float AvgV = VField[i]/numberPoints[i];
		float AvgW = WField[i]/numberPoints[i];
		for (auto &Velocity : VelocityVector)
		{
			if (Velocity.index == i)
			{
				Temperature[i] = Temperature[i] + pow(Velocity.u - AvgU,2) + pow(Velocity.v - AvgV,2) + pow(Velocity.w - AvgW,2);
			}
		}
		Temperature[i] = Temperature[i] / numberPoints[i];
	}
}

void eulerianFieldGranularTempSimulation::newFile()
{
	std::fstream file;
	file.open("PostFiles/"+FieldName+".txt", std::ios::out | std::ios::trunc);

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

void eulerianFieldGranularTempSimulation::writeFrame(int FrameNumber)
{
	std::fstream file;
	file.open("PostFiles/"+FieldName+".txt", std::ios::out | std::ios::app);

	file << FrameNumber << " :";
//	for (long double* value = Temperature; value != Temperature + NumberHeights*NumberWidths; ++value)
//		file << " " << *value;
	for (int i = 0; i < NumberCells; i++)
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

void eulerianFieldGranularTempSimulation::addParticle(float u, float v, float w, float x, float y, float z)
{
	int heightIndex = 0, widthIndexX = 0, widthIndexY = 0;
	
	for (; heightIndex < NumberHeights; ++heightIndex)
	{
		if ( z < Heights[heightIndex])
		{
			break;
		}
	}
	heightIndex--;
	for (; widthIndexY < NumberWidths; ++widthIndexY)
	{
		if ( y < Widths[widthIndexY])
		{
			break;
		}
	}
	widthIndexY--;
	for (; widthIndexX <  NumberWidths; ++widthIndexX)
	{
		if ( x < Widths[widthIndexX] )
		{
			break;
		}
	}
	widthIndexX--;


//	std::cout << x << " , " << y << " , " << z << " : " << u << std::endl;
	UField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = UField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + u;
	VField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = VField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + v;
	WField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = VField[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + w;
	numberPoints[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX]++;
	VelocityVector.push_back(Velocity3(u,v,w,heightIndex*(NumberWidths*NumberWidths)+widthIndexY*NumberWidths+widthIndexX));
	//std::cout << heightIndex << " , " << widthIndex << " : " << Field[heightIndex*NumberWidths+widthIndex] << " , " << numberPoints[heightIndex*NumberWidths+widthIndex] << std::endl;
}

float eulerianFieldSimulation::cellTop(int fieldAdress)
{
	int height = fieldAdress / NumberWidths;
	if (height < (NumberHeights - 1))
		return Heights[height+1];

	return Heights[NumberHeights-1] + (Heights[1] - Heights[0]);
}

float eulerianFieldSimulation::cellBottom(int fieldAdress)
{
	int height = fieldAdress / NumberWidths;
	return Heights[height];
}

int eulerianFieldSimulation::fieldSize()
{
	return NumberHeights * NumberWidths;
}

void eulerianFieldPartFracSimulation::addParticleFraction(float r, float x, float y, float z)
{
	int heightIndex = 0, widthIndexX = 0, widthIndexY = 0;
	
	for (; heightIndex < NumberHeights; ++heightIndex)
	{
		if ( z < Heights[heightIndex])
		{
			break;
		}
	}
	heightIndex--;
	for (; widthIndexX < NumberWidths; ++widthIndexX)
	{
		if ( x < Widths[widthIndexX])
		{
			break;
		}
	}
	widthIndexX--;
	for (; widthIndexY < NumberWidths; ++widthIndexY)
	{
		if ( y < Widths[widthIndexY])
		{
			break;
		}
	}
	widthIndexY--;
	
	//std::cout << x << " , " << y << " : " << heightIndex << " , " << widthIndex << std::endl;
	
	float dz = Heights[1] - Heights[0];
	float Vcell = M_PI*(Width/2.0)*dz;
	
	if (z - Heights[heightIndex] < r && heightIndex > 0)
	{
		float h = z - Heights[heightIndex];
		float V1 = 1/3.0*M_PI*pow(r-h,2)*(2*r+h);
		float V2 = 4/3.0*M_PI*pow(r,3) - V1;
		Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + V2/Vcell;
		Field[(heightIndex-1)*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[(heightIndex-1)*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + V1/Vcell;
		return;
	}
	if (Heights[heightIndex-1] - z < r && heightIndex < NumberHeights-1)
	{
		float h = Heights[heightIndex+1] - z;
		float V1 = 1/3.0*M_PI*pow(r-h,2)*(2*r+h);
		float V2 = 4/3.0*M_PI*pow(r,3) - V1;
		Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + V2/Vcell;
		Field[(heightIndex+1)*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[(heightIndex+1)*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + V1/Vcell;
		return;
	}

	Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] = Field[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX] + 4/3.0*M_PI*pow(r,3)/Vcell;
	numberPoints[heightIndex*(NumberWidths*NumberWidths) + widthIndexY*NumberWidths + widthIndexX]++;
	//std::cout << heightIndex << " , " << widthIndex << " : " << Field[heightIndex*NumberWidths+widthIndex] << " , " << numberPoints[heightIndex*NumberWidths+widthIndex] << std::endl;
}
