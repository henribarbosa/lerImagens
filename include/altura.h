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

#include "hough.h"
#include "read_control.h"

void altura(cv::Mat* image, cv::Mat* exibir, int& height);

void firstPass(const char* path, int& bottom, int& right, int& left);

void plugs(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, std::vector<cv::Rect>& rectangles, int bottom, float scale);

void interface(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, int& interfacePosition);

void interfaceNonPerpendicular(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, cv::Point& leftInterface, cv::Point& rightInterface);

void interfaceMixing(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, cv::Point& leftInterface, cv::Point& rightInterface);



