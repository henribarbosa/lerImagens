#define _USE_MATH_DEFINES

#include "../include/hough.h"

Hough::Hough(cv::Mat* image, int angleLines, int heightLines)
	:originalImage(image), angleLines(angleLines), heightLines(heightLines)
{
	accumulator = cv::Mat::zeros(angleLines, heightLines, CV_32F);
	if (originalImage->type() != CV_32F)
		(*originalImage).convertTo(*originalImage, CV_32F);
}

void Hough::transform(double minAngleInput, double maxAngle)
{
	imageHeight = originalImage->rows;
	imageWidth = originalImage->cols;

	distanceStep = (float)imageWidth / (float)(heightLines - 1);
	minAngle = minAngleInput * M_PI / 180;

	// buffer of tangent values used after
	cv::AutoBuffer<float> tgAngles(angleLines);
	angleStep = (maxAngle * M_PI / 180 - minAngle) / (angleLines - 1); 
	float angle = minAngle;
	for (int i = 0; i < angleLines; angle += angleStep, ++i)
	{
		tgAngles[i] = std::tan(angle);
	}

	// populate accumulator
	float* ptr;
	for (int i = 0; i < imageHeight; i++)
	{
		ptr = originalImage->ptr<float>(i);
		for (int j = 0; j < imageWidth; j++)
		{
			if (ptr[j] == (float)255)
			{
				for (int n = 0; n < angleLines; n++)
				{
					int Y = (int)((j + i * tgAngles[n]) / distanceStep);
					//std::cout << Y << std::endl;
					if (Y >= 0 && Y < heightLines)
						accumulator.at<float>(n, Y)++;
				}
			}
		}
	}

	//cv::namedWindow("Hough", cv::WINDOW_NORMAL);
	//cv::resizeWindow("Hough", 1200, 50);
	//cv::imshow("Hough", accumulator);
	//cv::waitKey(0);
}

void Hough::FindInterface(int whitePixelsTotal)
{
	// sum of whites above the line;
	float* ptr;
	for (int n = 0; n < angleLines; ++n)
	{
		float sum = 0;
		ptr = accumulator.ptr<float>(n);
		for (int i = 0; i < heightLines; ++i)
		{
			sum += ptr[i];
			ptr[i] = sum;
		}
	}

	// adjust by the area to find the biggest possible separation of whites and blacks
	control PixelContBalance("thresholds.txt","HoughBalanceParticles");
	int AreaTotal = imageHeight * imageWidth;
	for (int n = 0; n < angleLines; ++n)
	{
		ptr = accumulator.ptr<float>(n);
		for (int i = 0; i < heightLines; ++i)
		{
			float A1 = (i * distanceStep - (imageHeight * std::tan(minAngle + n * angleStep)) / 2) * imageHeight;
			ptr[i] = std::max(AreaTotal - A1 - whitePixelsTotal + (1 + PixelContBalance.returnThreshold()) * ptr[i], A1 + PixelContBalance.returnThreshold() * whitePixelsTotal - (1 + PixelContBalance.returnThreshold()) * ptr[i]) / 10;
			//std::cout << "ptr: " << ptr[i] << " , A1: " << A1 << " , At: " << AreaTotal << " , Wt: " << whitePixelsTotal << std::endl;
			//float val = std::abs((float)ptr[i] / A1 - (float)(whitePixelsTotal - ptr[i]) / (float)(AreaTotal - A1));
			//std::cout << i << " , " << n << " : " << val << std::endl;
			//ptr[i] = (val < 1.0f) ? val : 0;
			//ptr[i] = i + n;
		}
	}
}

void Hough::showDivision(cv::Point& leftPoint, cv::Point& rightPoint)
{
	cv::Point maxPoint;
	double maxVal;
	cv::minMaxLoc(accumulator, nullptr, &maxVal, nullptr, &maxPoint);
	float Y = maxPoint.x * distanceStep;
	float n = maxPoint.y * angleStep;
	//std::cout << "Pos: " << Y << " angulo: " << 180 / M_PI * (minAngle + n) << " valor: " << maxVal << std::endl;

	leftPoint = cv::Point((int)Y, 0);
	rightPoint = cv::Point((int)(Y - std::tan(minAngle + n) * imageHeight), imageHeight);
	//std::cout << leftPoint << " , " << rightPoint << std::endl;
}




