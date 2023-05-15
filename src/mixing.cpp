#include "../include/mixing.h"

Mixing::Mixing(int bottom, int left, float scale)
	:bottom(bottom), left(left), scale(scale)
{
	// mixing lacey index
	control BedWidth("thresholds.txt", "BedWidth");
	float bedWidth = BedWidth.returnThreshold();
	
	MixingField = eulerianField(100,3,bottom*scale,bedWidth,"Mixing");
}

void Mixing::interfaceMixing(cv::Mat* exibir, cv::Point& leftInterface, cv::Point& rightInterface, int frame)
{
	// read particles
	std::ifstream file;
	std::string linha;
	file.open("Files/circulos.txt", std::ios::in);
	int circulos;
	file >> circulos;
	std::getline(file,linha);
	
//	std::vector<circles_data> particles;
	particles.clear();
	particles.reserve(circulos);

	float x, y, r, brightness;		
	bool plug;
	int type;
	for (int i = 0; i < circulos; ++i) {
		file >> x >> y >> r >> plug >> brightness >> type;
		//std::cout << x << " , " << y << std::endl;
		particles.push_back(circles_data(x, y, r, plug, brightness, type));
	}

	file.close();

	// add particles
	for (int i = 0; i < particles.size(); i++)
	{
		MixingField.addParticle(particles[i].type - 1, (bottom - particles[i].x)*scale, (left - particles[i].y)*scale);
	}

	MixingField.consolidateField();

	orderVectors(particles);

	// find interface
	findInterface(leftInterface, rightInterface, 2, exibir);
	cv::line(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,0), 3);
	findInterface(leftInterface, rightInterface, 1, exibir);
	cv::line(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,255), 3);


	MixingField.writeFrame(frame);
}

void Mixing::findInterface(cv::Point& leftInterface, cv::Point& rightInterface, int type, cv::Mat* exibir)
{
//	leftInterface.y = 0;
//	rightInterface.y = exibir->rows;
	int dy = exibir->rows;

	if ( type == 2 )
	{
//		leftInterface.x = exibir->cols;
//		rightInterface.x = exibir->cols;

		int found = 0;
		for (int i = particles.size()-1; i>=0; i--)
//		for ( auto circle : particles )
		{
			circles_data circle = particles[i];
			if ( circle.type != type)
				continue;

			cv::Point testPoint = cv::Point(circle.x,circle.y);

			if ( found < 1 )
			{
				leftInterface = testPoint;
				found++;
			}
			else if ( found < 2 )
			{
				rightInterface = testPoint;
				found++;
			}

			if ( (testPoint.x - leftInterface.x) - (rightInterface.x - leftInterface.x)/(rightInterface.y - 1.0*leftInterface.y)*(testPoint.y - leftInterface.y) < 0)
			{
				float mLeft = (testPoint.x - leftInterface.x) / (1.0 * (testPoint.y - leftInterface.y));
				float mRight= (testPoint.x - rightInterface.x) / (1.0 * (testPoint.y - rightInterface.y));
				if (std::abs(mLeft) < std::abs(mRight))
				{
//					rightInterface.x = (int)(leftInterface.x + mLeft * dy);
					rightInterface = testPoint;
				}
				else
				{
//					leftInterface.x = (int)(rightInterface.x - mRight * dy);
					leftInterface = testPoint;
				}

			}
		}


	}

	if ( type == 1 )
	{
//		leftInterface.x = 0;
//		rightInterface.x = 0;

		int found = 0;
		for (int i = 0; i < particles.size(); i++)
		{
			circles_data circle = particles[i];
			if ( circle.type != type)
				continue;

			cv::Point testPoint = cv::Point(circle.x,circle.y);

			if ( found < 1 )
			{
				leftInterface = testPoint;
				found++;
			}
			else if ( found < 2 )
			{
				rightInterface = testPoint;
				found++;
			}

			if ( (testPoint.x - leftInterface.x) - (rightInterface.x - leftInterface.x)/(rightInterface.y - 1.0*leftInterface.y)*(testPoint.y - leftInterface.y) > 0)
			{
				float mLeft = (testPoint.x - leftInterface.x) / (1.0 * (testPoint.y - leftInterface.y));
				float mRight= (testPoint.x - rightInterface.x) / (1.0 * (testPoint.y - rightInterface.y));
				if (std::abs(mLeft) < std::abs(mRight))
				{
//					rightInterface.x = (int)(leftInterface.x + mLeft * dy);
					rightInterface = testPoint;
				}
				else
				{
//					leftInterface.x = (int)(rightInterface.x - mRight * dy);
					leftInterface = testPoint;
				}

			}
		}
	}

	if ( rightInterface.y != leftInterface.y )
		leftInterface.x = (int) (leftInterface.x - leftInterface.y * (rightInterface.x - leftInterface.x) / (rightInterface.y - leftInterface.y));
	else
		leftInterface.x = (leftInterface.x > rightInterface.x) ? leftInterface.x : rightInterface.x;

	leftInterface.y = 0;

	if ( rightInterface.y != leftInterface.y )
		rightInterface.x = (int) (leftInterface.x + (dy - leftInterface.y) * (rightInterface.x - leftInterface.x) / (rightInterface.y - leftInterface.y));
	else
		rightInterface.x = (leftInterface.x > rightInterface.x) ? leftInterface.x : rightInterface.x;

	rightInterface.y = dy;
}

template <typename T>
void merge(int* array, int const left, int const mid, int const right, std::vector<T>& vector)
{
	auto const subArrayOne = mid - left + 1;
	auto const subArrayTwo = right - mid;

	// Create temp arrays
	auto *leftArray = new int[subArrayOne], *rightArray = new int[subArrayTwo];

	// Copy data to temp arrays leftArray[] and rightArray[]
	for (auto i = 0; i < subArrayOne; i++)
		leftArray[i] = array[left + i];
	for (auto j = 0; j < subArrayTwo; j++)
		rightArray[j] = array[mid + 1 + j];

	auto indexOfSubArrayOne = 0, // Initial index of first sub-array
	indexOfSubArrayTwo = 0; // Initial index of second sub-array
	int indexOfMergedArray = left; // Initial index of merged array

	// Merge the temp arrays back into array[left..right]
	while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
		if (vector[leftArray[indexOfSubArrayOne]].x <= vector[rightArray[indexOfSubArrayTwo]].x) {
			array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
			indexOfSubArrayOne++;
		}
		else {
			array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
			indexOfSubArrayTwo++;
		}
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// left[], if there are any
	while (indexOfSubArrayOne < subArrayOne) {
		array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
		indexOfSubArrayOne++;
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// right[], if there are any
	while (indexOfSubArrayTwo < subArrayTwo) {
		array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
		indexOfSubArrayTwo++;
		indexOfMergedArray++;
	}
	delete[] leftArray;
	delete[] rightArray;
}

// begin is for left index and end is
// right index of the sub-array
// of arr to be sorted */
template <typename T>
void mergeSort(int* array, int const begin, int const end, std::vector<T>& vector)
{
	if (begin >= end)
		return; // Returns recursively

	auto mid = begin + (end - begin) / 2;
	mergeSort(array, begin, mid, vector);
	mergeSort(array, mid + 1, end, vector);
	merge(array, begin, mid, end, vector);
}

template <typename T>
void orderVectors(std::vector<T>& vector)
{
	int* labelsArray = new int[vector.size()];
	for (int i = 0; i < vector.size(); ++i)
	{
		labelsArray[i] = i;
	}

	mergeSort(labelsArray, 0, vector.size()-1, vector);

	std::vector<T> returnVector;
	returnVector.reserve(vector.size());
	for (int i = 0; i < vector.size(); i++)
	{
		returnVector.push_back(vector[labelsArray[i]]);
	}

	vector = returnVector;

//	std::vector<int> returnParticles;
//	std::vector<T> returnX, returnY, returnZ;
//	for (int i = 0; i < particles.size(); ++i)
//	{
//		returnParticles.push_back(particles[labelsArray[i]]);
//		returnX.push_back(x[labelsArray[i]]);
//		returnY.push_back(y[labelsArray[i]]);
//		returnZ.push_back(z[labelsArray[i]]);
//	}
//
//	particles = returnParticles;
//	x = returnX;
//	y = returnY;
//	z = returnZ;
}
