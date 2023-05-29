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
	std::vector<cv::Point> CirculosTipo1;
	std::vector<cv::Point> CirculosTipo2;

	// read particles
	std::ifstream file;
	std::string linha;
	file.open("Files/circulos.txt", std::ios::in);
	int circulos;
	file >> circulos;
	std::getline(file,linha);

	//write file
	std::ofstream writeFile;
	writeFile.open("Files/mixing_number.txt", std::ios::app);
	
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

	particlesType1 = cv::Mat::zeros(exibir->size(), CV_8UC1);
	particlesType2 = cv::Mat::zeros(exibir->size(), CV_8UC1);

	// add particles
	for (int i = 0; i < particles.size(); i++)
	{
		MixingField.addParticle(particles[i].type - 1, (bottom - particles[i].x)*scale, (left - particles[i].y)*scale);
		if (particles[i].type == 1) 
		{
			CirculosTipo1.push_back(cv::Point(particles[i].x, particles[i].y));
			particlesType1.at<uchar>(particles[i].y, particles[i].x) = 1;
		}
		else if (particles[i].type == 2) 
		{
			CirculosTipo2.push_back(cv::Point(particles[i].x, particles[i].y));
			particlesType2.at<uchar>(particles[i].y, particles[i].x) = 1;
		}
	}

//	std::cout << CirculosTipo1.size() << " " << CirculosTipo2.size() << std::endl;

	writeFile << cv::countNonZero(particlesType1) << " " << cv::countNonZero(particlesType2) << " ";

	MixingField.consolidateField();

	orderVectors_height(particles);

	// find interface
//	findInterface(leftInterface, rightInterface, 2, exibir);
//	cv::line(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,0), 3);

//	std::vector<cv::Point> hull;
//	cv::convexHull(CirculosTipo1, hull);
//	std::vector<std::vector<cv::Point>> hullShow; hullShow.push_back(hull);
//	cv::drawContours(*exibir, hullShow, 0, cv::Scalar(0,255,0), 3);

//	cv::convexHull(CirculosTipo2, hull);
//	hullShow.clear(); hullShow.push_back(hull);
//	cv::drawContours(*exibir, hullShow, 0, cv::Scalar(0,255,255), 3);

//	findInterface(leftInterface, rightInterface, 1, exibir);
//	cv::line(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,255), 3);
	std::vector<std::vector<cv::Point>> concaveShow;
	concaveShow.push_back(concaveHull(1));
	cv::drawContours(*exibir, concaveShow, 0, cv::Scalar(255,255,0), 3);
	
	hullType1 = cv::Mat::zeros(exibir->size(), CV_8UC1);
	cv::drawContours(hullType1, concaveShow, 0, cv::Scalar(255), -1);
//	cv::bitwise_and(hullType1,*exibir,*exibir);

	concaveShow.clear();
	concaveShow.push_back(concaveHull(2));
	cv::drawContours(*exibir, concaveShow, 0, cv::Scalar(0,255,255), 3);

	hullType2 = cv::Mat::zeros(exibir->size(), CV_8UC1);
	cv::drawContours(hullType2, concaveShow, 0, cv::Scalar(255), -1);

	// particles in different region
	cv::bitwise_and(particlesType1, hullType2, particlesType1);
	cv::bitwise_and(particlesType2, hullType1, particlesType2);
	writeFile << cv::countNonZero(particlesType1) << " " << cv::countNonZero(particlesType2) << std::endl;

	MixingField.writeFrame(frame);
}

std::vector<cv::Point> Mixing::concaveHull(int type)
{
	std::vector<circles_data> particles_type;
	for (circles_data circle : particles) {
//		std::cout << circle.type << " ";
		if (circle.type == type) {
			particles_type.push_back(circle);
		}
	}

//	std::cout << "Ok" << particles_type.size() << std::endl;

	circles_data reference = particles_type[0];

//	std::cout << "Ok" << std::endl;

	for (int i = 0; i < particles_type.size()-1; i++) {
		particles_type[i] = particles_type[i+1];
	}
	particles_type.pop_back();

	orderVectors_angle(particles_type, reference);
//	std::cout << compare_angle(particles_type[0],particles_type[10],reference) << std::endl;

	std::vector<circles_data> particles_stack;
	particles_stack.push_back(reference);

	control DotThreshold("thresholds.txt", "ConcaveDotThreshold");
	float threshold = DotThreshold.returnThreshold();

	for (int i = 0; i < particles_type.size(); i++) {
		while (particles_stack.size() > 1){
			if (left_turn(particles_stack[particles_stack.size()-2],particles_stack[particles_stack.size()-1],particles_type[i]) and not 
				large_turn(particles_stack[particles_stack.size()-2], particles_stack[particles_stack.size()-1], particles_type[i], threshold)) {
				particles_stack.pop_back();
			}
			else
				break;
		}
		particles_stack.push_back(particles_type[i]);
	}
	particles_stack.push_back(reference);

	std::vector<cv::Point> particles_return;
	for (int i = 0; i < particles_stack.size(); i++) {
		particles_return.push_back(cv::Point(particles_stack[i].x, particles_stack[i].y));
	}

	return particles_return;

}

bool left_turn(circles_data base, circles_data atual, circles_data teste)
{
	float dx1 = atual.x - base.x;
	float dy1 = atual.y - base.y;
	float dx2 = teste.x - atual.x;
	float dy2 = teste.y - atual.y;

	float vectorProduct = dx1*dy2 - dx2*dy1;
//	std::cout << vectorProduct << std::endl;
	return (vectorProduct > 0) ? true : false;
}

bool large_turn(circles_data base, circles_data atual, circles_data teste, float threshold)
{
	float dx1 = 1.0*(atual.x - base.x);
	float dy1 = 1.0*(atual.y - base.y);
	float dx2 = 1.0*(teste.x - atual.x);
	float dy2 = 1.0*(teste.y - atual.y);

	float dotProduct = (float)(dx1*dx2 + dy1*dy2) / ( std::sqrt(pow(dx1,2) + pow(dy1,2)) * std::sqrt(pow(dx2,2) + pow(dy2,2)) );
//	std::cout << dotProduct << std::endl;
	return (dotProduct > threshold) ? true : false;

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
void merge_height(int* array, int const left, int const mid, int const right, std::vector<T>& vector)
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
		if (compare_height(vector[leftArray[indexOfSubArrayOne]], vector[rightArray[indexOfSubArrayTwo]])) {
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
void mergeSort_height(int* array, int const begin, int const end, std::vector<T>& vector)
{
	if (begin >= end)
		return; // Returns recursively

	auto mid = begin + (end - begin) / 2;
	mergeSort_height(array, begin, mid, vector);
	mergeSort_height(array, mid + 1, end, vector);
	merge_height(array, begin, mid, end, vector);
}

template <typename T>
void orderVectors_height(std::vector<T>& vector)
{
	int* labelsArray = new int[vector.size()];
	for (int i = 0; i < vector.size(); ++i)
	{
		labelsArray[i] = i;
	}

	mergeSort_height(labelsArray, 0, vector.size()-1, vector);

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

template <typename T>
void merge_angle(int* array, int const left, int const mid, int const right, std::vector<T>& vector, T reference)
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
		int comp = compare_angle(vector[leftArray[indexOfSubArrayOne]], vector[rightArray[indexOfSubArrayTwo]], reference);
		if (comp == 1) {
			array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
			indexOfSubArrayOne++;
		}
		else if (comp == -1) {
			if (vector[leftArray[indexOfSubArrayOne]].x < vector[rightArray[indexOfSubArrayTwo]].x) {
				array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
				indexOfSubArrayOne++;
			}
			else {
				array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
				indexOfSubArrayTwo++;
			}
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
void mergeSort_angle(int* array, int const begin, int const end, std::vector<T>& vector, T reference)
{
	if (begin >= end)
		return; // Returns recursively

	auto mid = begin + (end - begin) / 2;
	mergeSort_angle(array, begin, mid, vector, reference);
	mergeSort_angle(array, mid + 1, end, vector, reference);
	merge_angle(array, begin, mid, end, vector, reference);
}

template <typename T>
void orderVectors_angle(std::vector<T>& vector, T reference)
{
	int* labelsArray = new int[vector.size()];
	for (int i = 0; i < vector.size(); ++i)
	{
		labelsArray[i] = i;
	}

	mergeSort_angle(labelsArray, 0, vector.size()-1, vector, reference);

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
bool compare_height(circles_data c1, circles_data c2)
{
	return (c1.x >= c2.x);
}

int compare_angle(circles_data c1, circles_data c2, circles_data referenece)
{
	float dx1 = c1.x - referenece.x;
	float dx2 = c2.x - referenece.x;
	float dy1 = c1.y - referenece.y;
	float dy2 = c2.y - referenece.y;

	float cos1 = dy1 / std::sqrt(std::pow(dx1,2) + std::pow(dy1,2));
	float cos2 = dy2 / std::sqrt(std::pow(dx2,2) + std::pow(dy2,2));
//	std::cout << cos1 << " , " << cos2 << std::endl;

	if (cos1 == cos2){
		return -1;
	}
	else if (cos1 < cos2){
		return 1;
	}
	else {
		return 0;
	}
}
