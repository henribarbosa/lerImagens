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
#include <thread>
#include <future>
#include <mutex>

#include "circles_data.h"
#include "read_control.h"

void circulos(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, std::vector<cv::Rect>& plugs);

cv::Mat quantizeImage(cv::Mat* image, int quantizationColors);

//accept only masks to speed up calculations
void generalMoviment(cv::Mat* image, cv::Mat* exibir, cv::Rect* bed, std::vector<cv::Rect>& plugs);

