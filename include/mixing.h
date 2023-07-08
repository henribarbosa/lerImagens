#include "read_control.h"
#include "eulerianField.h"
#include "circles_data.h"
//#include "math_functions.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <vector>

// calculate mixing index of the bed

class Mixing
{
private:
		int bottom, left; // bed size
		float scale; // bed scale
		void findInterface(cv::Point& leftInterface, cv::Point& rightInterface, int type, cv::Mat* exibir); // look for interface
		std::vector<cv::Point> concaveHull(int type); // region that encompass all particles of a type
		cv::Mat particlesType1, particlesType2; // binary representation of the 2 particles types, a 1 for each particle in its center
		cv::Mat hullType1, hullType2; // binary representation of the 2 regions
public:

		eulerianField MixingField; // mixing saved in eulerian cells
		std::vector<circles_data> particles; // particles positions
		
		Mixing(int bottom, int left, float scale); // start the field
		void interfaceMixing(cv::Mat* exibir, cv::Point& leftInterface, cv::Point& rightInterface, int frame); // another measure of mixing
};

// functions for ordering vectors
template <typename T> void merge_height(int* array, int const left, int const mid, int const right, std::vector<T>& vector);

template <typename T> void merge_angle(int* array, int const left, int const mid, int const right, std::vector<T>& vector, T reference);

template <typename T> void mergeSort_height(int* array, int const begin, int const end, std::vector<T>& vector);

template <typename T> void mergeSort_angle(int* array, int const begin, int const end, std::vector<T>& vector, T reference);

template <typename T> void orderVectors_height(std::vector<T>& particles);

template <typename T> void orderVectors_angle(std::vector<T>& particles, T reference);

// functions to build the concave hull, finding the higher particles and comparing the turns to aprove the hull construction
bool compare_height(circles_data c1, circles_data c2);

int compare_angle(circles_data c1, circles_data c2, circles_data reference);

bool left_turn(circles_data base, circles_data atual, circles_data teste);

bool large_turn(circles_data base, circles_data atual, circles_data teste, float threshold);
