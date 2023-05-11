#include "../include/circulos.h"


void circulos(cv::Mat* image, cv::Mat* exibir, cv::Rect& bed, std::vector<cv::Rect>& rectangles)
{
	// adjust bed to not consider borders, too much errors
	bed.y = bed.y + 5;
	bed.height = bed.height - 10;
	bed.width = bed.width - 5;

	cv::Mat figure = (*image)(bed);

	cv::GaussianBlur(figure,figure,cv::Size(5,5),0);
	cv::Mat1b KernelLocalMaxima(cv::Size(9,9),1u);
	KernelLocalMaxima.at<uchar>(4,4) = 0u;
	cv::Mat ImageLocalMaxima;
	cv::dilate(figure, ImageLocalMaxima, KernelLocalMaxima);
	cv::Mat LocalMaxima = (figure > (ImageLocalMaxima - 1));

	cv::Mat1b KernelLocalMinima(cv::Size(5,5),1u);
	KernelLocalMinima.at<uchar>(2,2) = 0u;
	cv::Mat ImageLocalMinima;
	cv::erode(figure, ImageLocalMinima, KernelLocalMinima);
	cv::Mat LocalMinima = (figure < (ImageLocalMinima));

	//cv::Mat1b KernelCleanImage(cv::Size(5,5),1u);
	//cv::erode(figure, figure, KernelCleanImage);
	//cv::dilate(figure, figure, KernelCleanImage);
	//KernelCleanImage = cv::Mat1b(cv::Size(3,3),1u);
	//cv::erode(figure, figure, KernelCleanImage);
	//cv::threshold(figure,figure,55,250,0);

	cv::Mat1b filter;
	//cv::Mat1b KernelCleanImage(cv::Size(5,5),1u);
	//cv::erode(figure, filter, KernelCleanImage);
	//cv::dilate(filter, filter, KernelCleanImage);
	//KernelCleanImage = cv::Mat1b(cv::Size(3,3),1u);
	//cv::erode(figure, figure, KernelCleanImage);
	control CirclesThreshold("thresholds.txt","CirclesThreshols");
	cv::threshold(figure,filter,CirclesThreshold.returnThreshold(),250,0);

	control typeIdentifier("thresholds.txt", "LowerThreshold");
	float lowerTypeThreshold = typeIdentifier.returnThreshold();

	control typeIdentifier2("thresholds.txt", "UpperThreshold");
	float upperTypeThreshold = typeIdentifier2.returnThreshold();

	control CircleMinimumThreshold("thresholds.txt","CircleMinimumValueThreshold");
	uchar threshold = CircleMinimumThreshold.returnThreshold();
	
	//std::mutex ImageLock;
	//LocalMinima.forEach<uchar>(
	//	[&figure, &ImageLock](uchar& pixel, const int* position)
	//	{
	//		if(pixel > 0)
	//		{
	//			std::lock_guard<std::mutex> lock(ImageLock);
	//			cv::circle(figure,cv::Point(position[1],position[0]),1,cv::Scalar(255,0,0));
	//		}
	//	});

	std::vector<circles_data> circlesVector;
	std::mutex circlesLock;

	LocalMaxima.forEach<uchar>(
		[&figure, &LocalMinima, &circlesVector, &circlesLock, &filter, &lowerTypeThreshold, &upperTypeThreshold, &threshold](uchar& pixel, const int* position)
		{
			if (pixel > 0)
			{
				//cv::circle(figure,cv::Point(position[1],position[0]),1,cv::Scalar(55,0,0));

				//for (int rad = 3; rad < 11; rad++)
				//{
				//	cv::Mat1b circleImage(LocalMinima.size(),0u);
				//	for (int i = -rad; i <= rad; i++)
				//	{
				//		for (int j = -rad; j <= rad; j++)
				//		{
				//			int xPos = position[0] + i;
				//			int yPos = position[1] + j;
				//			if (xPos < 0 || xPos >= figure.rows)
				//				continue;
				//			if (yPos < 0 || yPos >= figure.cols)
				//				continue;
//
				//			if (sqrt(pow(i,2)+pow(j,2)) <= rad)
				//			{
				//				circleImage.at<uchar>(xPos,yPos) = 1u;
				//			}
				//		}
				//	}

				//	cv::Mat sumMinimum(circleImage);
				//	sumMinimum.mul(LocalMinima/255);
				//	//cv::normalize(sumMinimum,sumMinimum,0,1);
				//	cv::Mat sumValue(circleImage);
				//	sumValue.mul(valueMinima/255);
				//	//cv::normalize(sumValue,sumValue,0,1);
				//	sumMinimum = sumMinimum + sumValue;
//
				//	int minima = cv::sum(sumMinimum)[0];

				//	if (minima > 75)
				//	{
				//		radius = rad;
				//		break;
				//	}
				//}


				int radius = 100;
				for (int i = -10; i < 11; i++)
				{
					for (int j = -10; j < 10; j++)
					{
						int xPos = position[0] + i;
						int yPos = position[1] + j;
						if (xPos < 0 || xPos >= figure.rows)
							continue;
						if (yPos < 0 || yPos >= figure.cols)
							continue;
						
						bool MinimumPoint = LocalMinima.at<uchar>(xPos,yPos) > 0u;
						if (MinimumPoint)
						{
							int pointDistance = (int)sqrt(pow(i,2)+pow(j,2));
							if (pointDistance < 1) continue;
							radius = (pointDistance < radius)? pointDistance : radius;
							continue;
						}
						
						uchar pixelValue = figure.at<uchar>(xPos,yPos);
						if (pixelValue < threshold)
						{
							int pointDistance = (int)sqrt(pow(i,2)+pow(j,2));
							radius = (pointDistance < radius)? pointDistance : radius;
							continue;
						}
					}
				}

				// left, right, up, bottom
				//int directionSize[4] = {position[0],position[0],position[1],position[1]};
				//uchar directionBuffer[4] = {pixel,pixel,pixel,pixel};

				//while (true)
				//{
				//	uchar value = figure.at<uchar>(directionSize[0]-1,position[1]);
				//	if (directionBuffer[0] >= value && value > threshold)
				//	{
				//		directionSize[0]--;
				//		directionBuffer[0] = value;
				//	}
				//	else
				//		break;
				//}
				//while (true)
				//{
				//	uchar value = figure.at<uchar>(directionSize[1]+1,position[1]);
				//	if (directionBuffer[1] >= value && value > threshold)
				//	{
				//		directionSize[1]++;
				//		directionBuffer[1] = value;
				//	}
				//	else 
				//		break;
				//}
				//while (true)
				//{
				//	uchar value = figure.at<uchar>(position[0], directionSize[2]+1);
				//	if (directionBuffer[1] >= value && value > threshold)
				//	{
				//		directionSize[2]++;
				//		directionBuffer[2] = value;
				//	}
				//	else 
				//		break;
				//}
				//while (true)
				//{
				//	uchar value = figure.at<uchar>(position[0],directionSize[3]-1);
				//	if (directionBuffer[3] >= value && value > threshold)
				//	{
				//		directionSize[3]--;
				//		directionBuffer[3] = value;
				//	}
				//	else 
				//		break;
				//}

				//int y1 = position[0] - directionSize[0];
				//int y2 = directionSize[1] - position[0];
				//int x1 = directionSize[2] - position[1];
				//int x2 = position[1] - directionSize[3];
				//int r = (int)sqrt((pow(x1,2) + pow(x2,2) + pow(y1,2) + pow(y2,2)) / 4.0);
				//int centerY = (int)(directionSize[0] + directionSize[1]) / 2;
				//int centerX = (int)(directionSize[2] + directionSize[3]) / 2;

				if (radius < 3 || radius > 10)
					return;
				{
					std::lock_guard<std::mutex> lock(circlesLock);
					bool foundOverlap = false;
					for (int i = 0; i < circlesVector.size(); i++)
					{
						float centerDistance = sqrt(pow(position[1]-circlesVector[i].x,2)+pow(position[0]-circlesVector[i].y,2));
						float overlap = radius + circlesVector[i].r - centerDistance;

						if (overlap > 1)
						{
							if (radius > circlesVector[i].r || centerDistance < 3)
							{
								radius -= (int)overlap/2;
								if (radius < 4)
								{
									foundOverlap = true;
									break;
								}
							}
							else 
							{
								circlesVector[i].r -= (int)overlap/2;
								if (circlesVector[i].r < 4)
								{
									circlesVector.erase(circlesVector.begin()+i);
									break;
								}
							}
						}

						//if (overlap > 1 && centerDistance < 15)
						//{
						//	foundOverlap = true;
							
						//	cv::Mat1b sumCurrent(LocalMinima.size(),0u);
						//	cv::Mat1b sumOther(LocalMinima.size(),0u);

						//	cv::circle(sumCurrent, cv::Point(position[1],position[0]), radius-1, cv::Scalar(1,1,1), -1);
						//	cv::circle(sumOther, cv::Point(circlesVector[i].x, circlesVector[i].y), circlesVector[i].r-1, cv::Scalar(1,1,1), -1);

						//	sumCurrent.mul(filter);
						//	sumOther.mul(filter);

						//	double minCurrent, maxCurrent;
						//	double minOther, maxOther;
						//	cv::minMaxLoc(sumCurrent,&minCurrent,&maxCurrent);
						//	cv::minMaxLoc(sumOther,&minOther,&maxOther);
						//	//if( maxCurrent > maxOther)
						//	if (cv::sum(sumCurrent)[0]/pow(radius,2) > cv::sum(sumOther)[0]/pow(circlesVector[i].r,2))
						//	{
						//		circlesVector[i].x = position[1];
						//		circlesVector[i].y = position[0];
						//		circlesVector[i].r = radius;
						//		break;
						//	}

							//int radiusMerged = (int)( (centerDistance + radius + circlesVector[i].r) / 2 );

							//if (radiusMerged > 10) continue;

							//int centerX = (int)(position[1] + (centerDistance/(radiusMerged-radius)*(circlesVector[i].x - position[1])));
							//int centerY = (int)(position[0] + (centerDistance/(radiusMerged-radius)*(circlesVector[i].y - position[0])));
							//circlesVector[i].x = centerX;
							//circlesVector[i].y = centerY;
							//circlesVector[i].r = radiusMerged;
							//return;
						//}
					}
					if (not foundOverlap)
					{
						int sum = 0;
						for (int i = -1; i <= 1; i++)
						{
							sum = sum + (int)figure.at<uchar>(position[0] + i,position[1]);
						}
						for (int i = -1; i <= 1; i++)
						{
							sum = sum + (int)figure.at<uchar>(position[0],position[1] + 1);
						}
						float avg = (float)sum / 6.0;
						int type = 0;
						if (avg < lowerTypeThreshold)
						{
							type = 1;
						}
						else if (avg > upperTypeThreshold) 
						{
							type = 2;
						}

						circlesVector.push_back(circles_data(position[1],position[0],radius,false,avg,type));
						//cv::circle(figure,cv::Point(position[1],position[0]),radius,cv::Scalar(127,0,0));
					}
				}
			}
		}
	);

	//cv::namedWindow("Aveg",cv::WINDOW_NORMAL);
	//cv::imshow("Aveg", figure);

	//cv::Mat gray;
	//medianBlur((*image)(bed), gray, 5);
	//std::vector<cv::Vec3f> circles, prohibted;
	//cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1,
	//	6,  // change this value to detect circles with different distances to each other
	//	40, 5, 4, 6 // change the last two parameters
		// (min_radius & max_radius) to detect larger circles
	//);

	//int thresholdValue = 60;
	//for (std::vector<cv::Vec3f>::iterator circle = circles.begin(); circle != circles.end(); ++circle)
	//{
	//	cv::Point center((*circle)[0] + bed.x, (*circle)[1] + bed.y);
	//	if (image->at<uchar>(center) < thresholdValue)
	//	{
	//		prohibted.push_back(*circle);
	//	}
	//}

	//for (std::vector<cv::Vec3f>::iterator item = prohibted.begin(); item != prohibted.end(); ++item)
	//{
	//	circles.erase(std::remove(circles.begin(), circles.end(), *item), circles.end());
	//}

	for (int i = 0; i < circlesVector.size(); ++i)
	{
		circlesVector[i].x += bed.x;
		circlesVector[i].y += bed.y;
	}

	//std::vector<bool> inPlug; inPlug.resize(circles.size(), false);
	for(int i = 0; i < circlesVector.size(); i++)
	{
		//cv::Vec3i c = circles[i];
		//cv::Point center = cv::Point(c[0], c[1]);
		//int radius = c[2];

		for (std::vector<cv::Rect>::iterator plug = rectangles.begin(); plug != rectangles.end(); ++plug)
		{
			if (circlesVector[i].inPlug)
			{
				continue;
			}
			if (plug->x + bed.x < circlesVector[i].x && circlesVector[i].x < plug->x + plug->width + bed.x)
			{
				circlesVector[i].inPlug = true;
			}
		}

		if (circlesVector[i].type == 0)
		{
			int type1Bin = 0;
			int type2Bin = 0;
			for (auto circle : circlesVector)
			{
				if (circle.dist(circle,circlesVector[i]) < 15)
				{
					if (circle.type == 1) { type1Bin++;}
					else if (circle.type == 2) { type2Bin++;}
				}
			}
			circlesVector[i].type = (type1Bin >= type2Bin) ? 1 : 2;
		}

		if (circlesVector[i].type == 1) {
			cv::circle(*exibir, cv::Point(circlesVector[i].x,circlesVector[i].y), circlesVector[i].r, cv::Scalar(255,0,255), 1, cv::LINE_AA);
		}
		else if (circlesVector[i].type == 2) {
			cv::circle(*exibir, cv::Point(circlesVector[i].x,circlesVector[i].y), circlesVector[i].r, cv::Scalar(0,255,255), 1, cv::LINE_AA);
		}
	}

	std::ofstream file;
	file.open("Files/circulos.txt",std::ios::trunc);
	file << circlesVector.size() << " circulos" << std::endl;
	for (int i=0; i < circlesVector.size(); i++)
	{
		file << circlesVector[i].x << " " << circlesVector[i].y << " " << circlesVector[i].r << " " << circlesVector[i].inPlug << " " << circlesVector[i].brightness << " " << circlesVector[i].type << std::endl;
	}
	file.close();
}

void findBestCorrespondence(cv::Mat* image, cv::Rect* window, int index, std::vector<int>* bestCorrespondence)
{
	int bestValue = 0;
	
	for (int i = -10; i <= 10; ++i)
	{
		if (i + window->x < 0 || i >= image->cols - window->width)
			continue;
		cv::Rect movingWindow = *window + cv::Point(i, 0);
		cv::Mat multiplyResult;
		cv::bitwise_and((*image)(*window), (*image)(movingWindow), multiplyResult);
		int val = cv::sum(multiplyResult)[0];
		if (val > bestValue)
		{
			bestValue = val;
			(*bestCorrespondence)[index] = i;
		}
	}
}

void generalMoviment(cv::Mat* image, cv::Mat* exibir, cv::Rect* bed, std::vector<cv::Rect>& plugs)
{
	const int windowWidth = image->cols;
	const int windowHeight = image->rows;
	const int windowSize = 35;
	std::vector<int> plugsAnalisePoints; plugsAnalisePoints.resize(plugs.size(), 0);
	std::vector<int> bestCorrespondence; bestCorrespondence.resize(plugs.size(), 0);

	std::vector<std::future<void>> futureVector; int index = 0;
	for (std::vector<cv::Rect>::iterator plug = plugs.begin(); plug != plugs.end(); ++plug, ++index)
	{
		cv::Rect window;
		if (plug->width <= windowSize)
		{
			plugsAnalisePoints[index] = plug->x;
			window = cv::Rect(plugsAnalisePoints[index], 0, plug->width, windowHeight);
		}
		else
		{
			plugsAnalisePoints[index] = plug->x + (int)(plug->width / 2);
			window = cv::Rect(plugsAnalisePoints[index], 0, windowSize, windowHeight);
		}
		futureVector.push_back(std::async(std::launch::async, findBestCorrespondence, image, &window, index, &bestCorrespondence));
	}

	cv::Point translation(bed->x, bed->y);
	for (int i = 0; i < bestCorrespondence.size(); ++i)
	{
		cv::line(*exibir, cv::Point(plugsAnalisePoints[i], 0) + translation, cv::Point(plugsAnalisePoints[i] + bestCorrespondence[i], windowHeight) + translation, cv::Scalar(255, 255, 0), 3, 8);
	}
}

cv::Mat quantizeImage(cv::Mat* image, int quantizationColors)
{
	cv::Mat src = image->clone();  //cloning mat data
	cv::Mat data = cv::Mat::zeros(src.cols * src.rows, 3, CV_32F);  //Creating the matrix that holds all pixel data
	cv::Mat bestLabels, centers, clustered; //Returns from the K Means
//	std::vector<cv::Mat> bgr;    //Holds the BGR channels
//	cv::split(src, bgr);

	//Getting all pixels in the Data row column to be compatible with K Means
	for (int i = 0; i < src.cols * src.rows; i++)
	{
		data.at<float>(i, 0) = src.data[i] / 255.0;
		data.at<float>(i, 1) = src.data[i] / 255.0;
		data.at<float>(i, 2) = src.data[i] / 255.0;
	}

	int K = quantizationColors;    //Number of clusters
	cv::kmeans(data, K, bestLabels,
		cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 20, 0.1),
		3, cv::KMEANS_RANDOM_CENTERS, centers);

	centers = centers.reshape(3, centers.rows);
	data = data.reshape(3, data.rows);

	clustered = cv::Mat(src.rows, src.cols, CV_32F);

	cv::Vec3f* p = data.ptr<cv::Vec3f>();
	for (size_t i = 0; i < data.rows; i++)
	{
		int center_id = bestLabels.at<int>(i);
		p[i] = centers.at<cv::Vec3f>(center_id);
	}

	clustered = data.reshape(3, src.rows);
	return clustered;
}



