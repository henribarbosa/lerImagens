#include "../include/math_functions.h"

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
