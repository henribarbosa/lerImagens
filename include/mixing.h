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

class Mixing
{
private:
		int bottom, left;
		float scale;
		void findInterface(cv::Point& leftInterface, cv::Point& rightInterface, int type, cv::Mat* exibir);
public:

		eulerianField MixingField;
		std::vector<circles_data> particles;
		
		Mixing(int bottom, int left, float scale);
		void interfaceMixing(cv::Mat* exibir, cv::Point& leftInterface, cv::Point& rightInterface, int frame);
};

template <typename T> void merge(int* array, int const left, int const mid, int const right, std::vector<T>& vector);

template <typename T> void mergeSort(int* array, int const begin, int const end, std::vector<T>& vector);

template <typename T> void orderVectors(std::vector<T>& particles);

