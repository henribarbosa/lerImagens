#include "read_control.h"
#include "eulerianField.h"
#include "circles_data.h"

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
		void findInterface(cv::Point& leftInterface, cv::Point& rightInterface);
public:

		eulerianField MixingField;
		std::vector<circles_data> particles;
		
		Mixing(int bottom, int left, float scale);
		void interfaceMixing(cv::Mat* exibir, cv::Point& leftInterface, cv::Point& rightInterface, int frame);
};
