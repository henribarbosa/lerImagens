#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "read_control.h"

struct Hough
{
	cv::Mat accumulator;
	int imageHeight, imageWidth;
	int angleLines, heightLines;
	float angleStep, distanceStep;
	float minAngle;
	cv::Mat* originalImage;

	Hough(cv::Mat* image, int angleLines, int height);
	void transform(double minAngle, double maxAngle); //use degrees
	void FindInterface(int whitePixelsTotal);
	void showDivision(cv::Point& leftPoint, cv::Point& rightPoint);
};
