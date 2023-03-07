#include <opencv2/opencv.hpp>
#include <regex>

#include "../include/preProcess.h"

void MakeAveragePicture(std::vector<std::string> files)
{
	cv::namedWindow("Build mask", cv::WINDOW_NORMAL);
	cv::resizeWindow("Build mask", 1200,50);

	cv::Mat image = cv::imread(files[0], cv::IMREAD_GRAYSCALE);
	image.convertTo(image, CV_64F);
	cv::Mat average(image);
	cv::Mat temp;

	for (int i = 1; i < files.size(); i++)
	{
		std::cout << "File: " << files[i] << std::endl;

		temp = cv::imread(files[i], cv::IMREAD_GRAYSCALE);
		cv::imshow("Build mask", temp);
		temp.convertTo(temp, CV_64F);

		try {
			average += temp;
		}
		catch( cv::Exception &e){
			std::cerr << e.msg << std::endl;
			std::cin.get();
		}
	}

	average.convertTo(average, CV_8U, (1. / files.size()));
	//average.convertTo(average, CV_8UC1);

	cv::imshow("Build mask", average);
	cv::waitKey(0);

	cv::imwrite("mask.tif", average); //saves mask to subtract from files
}

void SubtractMask(std::vector<std::string> files)
{
	cv::namedWindow("Remove background", cv::WINDOW_NORMAL);
	cv::resizeWindow("Remove background", 1200,50);

	cv::Mat mask = cv::imread("mask.tif", cv::IMREAD_GRAYSCALE);
	
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image;

		try {
			image = cv::imread(files[i], cv::IMREAD_GRAYSCALE);
			image -= mask;
		}
		catch(cv::Exception &e){
			std::cerr << e.msg << std::endl;
			continue;
		}

		cv::imshow("Remove background", image);
		cv::waitKey(1);

		std::string test = files[i];

		std::regex pattern("(.*)\\/(\\w+)\\/(\\w+\\.\\w{3})");
		std::string outFile = std::regex_replace(test, pattern, "$1/Processed/$3");

		std::cout << "Writing: " << outFile << std::endl;
		cv::imwrite(outFile, image);

	}
}

void MakeBinary(std::vector<std::string> files)
{
	cv::namedWindow("Binary", cv::WINDOW_NORMAL);
	cv::resizeWindow("Binary", 1200, 50);

	cv::Mat image;

	for (int i = 0; i < files.size(); i++)
	{
		try
		{
			image = cv::imread(files[i], cv::IMREAD_GRAYSCALE);

			cv::threshold(image, image, 128, 255, 0);
			// threshold, white value, type=binary

			cv::imshow("Binary", image);
			cv::waitKey(0);
		}
		catch (cv::Exception &e)
		{
			std::cerr << e.msg << std::endl;
		}

	}

}

void RotateImages(std::vector<std::string> files)
{
	cv::namedWindow("Rotated images", cv::WINDOW_NORMAL);
	cv::resizeWindow("Rotated images", 1200,50);
	
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image;

		try {
			image = cv::imread(files[i], cv::IMREAD_GRAYSCALE);
			cv::Mat dst;
			//cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::bilateralFilter(image, dst, 7, 15, 15);
		}
		catch(cv::Exception &e){
			std::cerr << e.msg << std::endl;
			continue;
		}

		cv::imshow("Rotated images", image);
		cv::waitKey(1);

		std::string test = files[i];

		std::regex pattern("(.*)\\/(\\w+)\\/(\\w+\\.\\w{3})");
		std::string outFile = std::regex_replace(test, pattern, "$1/Processed/$3");

		std::cout << "Writing: " << outFile << std::endl;
		cv::imwrite(outFile, image);

	}
}




