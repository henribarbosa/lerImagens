#include "../include/altura.h"


void altura(cv::Mat* imagem, cv::Mat* exibir, int& height)
{
	double min, max;
	control HeightThreshold("thresholds.txt","HeightPixelThreshold");
	double corte = HeightThreshold.returnThreshold();

	for (int i = 100; i < exibir->cols - 100; i++)
	{
		//std::cout << i << " : " << media[i] << std::endl;		
		minMaxIdx(imagem->col(i), &min, &max);
		if (max >= corte){
			height = i;
			break;
		}
	}

	cv::line(*exibir, cv::Point(height, 0), cv::Point(height, exibir->rows), cv::Scalar(0, 255, 0), 3, 8);
}

void GaussianDifference(cv::Mat& input, cv::Mat& output, int sigma, double k)
// difference of gaussian blurs with sigma and k*sigma
{
	cv::Mat g1, g2;
	cv::GaussianBlur(input, g1, cv::Size(0, 0), sigma, sigma);
	cv::GaussianBlur(input, g2, cv::Size(0, 0), k * sigma, k * sigma);
	cv::subtract(g1, g2, output);
	output *= 4;
}

void firstPass(const char* path, int& bottom, int& right, int& left)
//set lines for bottom, right and left limits of the bed
{
	cv::namedWindow("Lines", cv::WINDOW_NORMAL);
	cv::resizeWindow("Lines", 1200, 50);

	cv::Mat image = imread(path, cv::IMREAD_GRAYSCALE), show;
	cv::cvtColor(image, show, cv::COLOR_GRAY2RGB);

	GaussianDifference(image, image, 1, 3);

	control MinimumSumThreshold("thresholds.txt","MinimumSumFindMargins");
	int minimumSum = MinimumSumThreshold.returnThreshold();

	for (int i = 0; i < image.rows; i++)
	{
		cv::Rect slide(0, i, image.cols, 1);

		cv::Scalar testSum = cv::sum(image(slide));
		if (testSum[0] > minimumSum)
		{
			right = i;
			break;
		}
	}

	for (int i = image.rows-1; i > 0; i--)
	{
		cv::Rect slide(0, i, image.cols, 1);

		cv::Scalar testSum = cv::sum(image(slide));
		if (testSum[0] > minimumSum)
		{
			left = i;
			break;
		}
	}

	for (int i = image.cols-1; i > 0; i--)
	{
		cv::Rect slide(i, 0, 1, image.rows);

		cv::Scalar testSum = cv::sum(image(slide));
		if (testSum[0] > minimumSum/10)
		{
			bottom = i;
			break;
		}
	}

	//std::cout << right << " , " << left << " , " << bottom << std::endl;

	cv::imshow("Lines", image);
	cv::waitKey(500);

	cv::line(show, cv::Point(0, right), cv::Point(show.cols, right), cv::Scalar(255, 0, 0),2,8);
	cv::line(show, cv::Point(0, left), cv::Point(show.cols, left), cv::Scalar(255, 0, 0), 2, 8);
	cv::line(show, cv::Point(bottom, 0), cv::Point(bottom, show.rows), cv::Scalar(255,255, 0),2,8);

	cv::imshow("Lines", show);
	cv::waitKey(500);
	cv::destroyWindow("Lines");
}

void plugs(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, std::vector<cv::Rect>& rectangles, int bottom, float scale)
{
	cv::Point offset(bed.x, bed.y);
	cv::Mat bedImage = (*image)(bed);
	cv::Mat blackWhite;

	control binaryImageThreshold("thresholds.txt","BinaryImagePlugs");
	cv::threshold(bedImage, blackWhite, binaryImageThreshold.returnThreshold() , 1, 0);
	//cv::imshow("Plugs", blackWhite);
	//cv::waitKey(0);

	control minimumPlugFraction("thresholds.txt","MinimumPlugFraction");
	control minimumPlugHeight("thresholds.txt","MinimumPlugHeight");
	float avgThreshold = minimumPlugFraction.returnThreshold();
	int buffer = 0, minHeightPlug = minimumPlugHeight.returnThreshold();
	// cut to find a packed bed, , minimum pixels of packed bed to be considered a plug
	bool first = false, plug = false;
	cv::Point firstPoint;

	for (int i = 0; i < bedImage.cols; i++)
	{
		cv::Rect slidingColumnWindow(i, 0, 1, bedImage.rows);
		float windowAvg = cv::sum(blackWhite(slidingColumnWindow))[0] / slidingColumnWindow.area();
		if (windowAvg >= avgThreshold)
		{
			buffer += 1;
			if (!first)
			{
				firstPoint = cv::Point(i, 0);
				first = true;
			}
			else
			{
				if (buffer >= minHeightPlug)
				{
					plug = true;
				}
			}
		}
		else
		{
			first = false;
			buffer = 0;
			if (plug)
			{
				bool merged = false;
				//for (int j = 0; j < rectangles.size(); j++)
				int j = rectangles.size()-1;
				if (rectangles.size() != 0)
				{
					if (firstPoint.x - (rectangles[j].x + rectangles[j].width) < minHeightPlug)
					{
						//std::cout << "Merged" << std::endl;
						rectangles[j].width = i - firstPoint.x;
						merged = true;
					}
				}
				if (not merged)
				{
					rectangles.push_back(cv::Rect(firstPoint, cv::Point(i, bedImage.rows)));
					plug = false;
				}
			}
		}
	}
	if (plug)
	{
		bool merged = false;
		//for (int j = 0; j < rectangles.size(); j++)
		int j = rectangles.size()-1;
		if (rectangles.size() != 0)
		{
			if (firstPoint.x - (rectangles[j].x + rectangles[j].width) < minHeightPlug)
			{
				//std::cout << "Merged" << std::endl;
				rectangles[j].width = bedImage.cols - firstPoint.x;
				merged = true;
			}
		}
		if (not merged)
		{
			rectangles.push_back(cv::Rect(firstPoint, cv::Point(bedImage.cols, bedImage.rows)));
		}
	}

	for (int i = 0; i < rectangles.size(); i++)
	{
		cv::rectangle(*exibir, rectangles[i] + offset, cv::Scalar(0, 0, 255), 2, 8);
	}

	std::ofstream file;
	file.open("Files/Plugs.txt", std::ios::app);
	file << rectangles.size() << " :";
	for (int i = 0; i < rectangles.size(); i++)
	{
		file << " " << (bottom - bed.x - rectangles[i].x - rectangles[i].width)*scale << " " << (bottom - bed.x - rectangles[i].x)*scale;
	}
	file << std::endl;
	file.close();
}

void interface(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, int& interfacePosition)
{
	cv::Mat process = (*image)(bed);
	cv::Mat mask;

	cv::inRange(process, 200, 255, mask);

	//cv::namedWindow("Mask", cv::WINDOW_NORMAL);
	//cv::resizeWindow("Mask", 1200, 50);
	//cv::imshow("Mask", mask*255);
	//cv::waitKey(0);

	bool inverted = false;
	int minHeightInverted = 50;
	int lastAbove = 0;

	for (int i = 0; i < process.cols; i++)
	{
		cv::Rect slidingWindow(i, 0, 1, process.rows);
		bool aboveThreshold = (cv::sum(mask(slidingWindow))[0] > 5) ? true : false;

		if (aboveThreshold)
		{
			lastAbove = i;
			if (i < minHeightInverted && !inverted)
				inverted = true;
			else if (!inverted)
			{
				interfacePosition = i + bed.x;
				cv::line(*exibir, cv::Point(interfacePosition, 0), cv::Point(interfacePosition, exibir->rows), cv::Scalar(255,0,0), 3, 8);
				break;
			}
		}
		else
		{
			if (i > minHeightInverted)
			{
				if (inverted && i - lastAbove > 15)
				{
					interfacePosition = lastAbove + bed.x;
					cv::line(*exibir, cv::Point(interfacePosition, 0), cv::Point(interfacePosition, exibir->rows), cv::Scalar(255, 0, 0), 3, 8);
					break;
				}
			}
		}
	}

}

void interfaceNonPerpendicular(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, cv::Point& leftInterface, cv::Point& rightInterface)
{
	cv::Mat process = (*image)(bed);
	cv::Mat mask;

	//cv::threshold(process, mask, 200, 1, 0);
	//cv::multiply(process, mask, process);
	control interfaceNonPerpendicularThreshold("thresholds.txt","InterfaceNonPerpendicularThreshold");
	cv::inRange(process, interfaceNonPerpendicularThreshold.returnThreshold(), 255, mask);
	Hough HoughTransform(&mask, 91, 301);
	HoughTransform.transform(-15, 15);
	HoughTransform.FindInterface(cv::countNonZero(mask));
	HoughTransform.showDivision(leftInterface, rightInterface);
	
	//adjust to full frame
	cv::Point tranlation(bed.x, bed.y);
	leftInterface += tranlation;
	rightInterface += tranlation;

	cv::line(*exibir, leftInterface, rightInterface, cv::Scalar(255, 0, 0), 3, 8);
	//cv::line(mask, leftInterface, rightInterface, cv::Scalar(1, 1, 1), 3, 8);

	//cv::namedWindow("Mask", cv::WINDOW_NORMAL);
	//cv::resizeWindow("Mask", 1200, 50);
	//cv::imshow("Mask", mask*255);
	//cv::waitKey(0);


}







