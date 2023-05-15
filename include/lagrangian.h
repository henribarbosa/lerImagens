#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>
#include <queue>
#include <thread>
#include <glob.h>

#include "circles_data.h"
#include "eulerianField.h"
#include "read_control.h"

class lagrangian
{
private:
	int last_label = 0; //to increase the number of labels
	int last_point = 0; //to increase the number of points
	int frame = 0; //number of processed frames
	std::vector<int> labels; //which label was assigned to the particles last frame	
	std::vector<bool> usedlabels; //which labels were already used
	std::vector<std::pair<float,float>> singlePoints; //new points, to see if does not show up later
	std::vector<std::pair<float,float>> possiblePoints; // possible points that were not assigned last pass
	std::vector<std::pair<float,float>> kalmanPoints; // possible points for the kalman filter
	std::vector<int> possiblePointsPositions; //positions in the list of the previous frame
	std::vector<std::pair<float,float>> kalmanMidlePoints;
	std::vector<int> kalmanLabel;
	std::fstream file;
	std::fstream pointsFile;
	
	float scale;
	int bottom = 0, left = 0;

	eulerianField VelocityX, VelocityY, massFluxX, massFluxY;
	eulerianFieldGranularTemp Temperature;
	
	void createFile();
public:
	void setScale(float scale);
	void setBoundaries(int bottom, int left);
	void startFields();
	void writePoint(const float x, const float y, const float z);
	void writeNewParticle();
	void writeToParticle(int& particle, const float x, const float y, const float z);
	void writeToMultipleParticles(std::vector<int>& orderedParticles, std::vector<float>& orderedX, std::vector<float>& orderedY, std::vector<float>& orderedZ);
	void writeToMultipleParticlesDouble(std::vector<int>& orderedParticles, std::vector<std::pair<float,float>>& orderedX, std::vector<std::pair<float,float>>& orderedY,
		std::vector<std::pair<float,float>>& orderedZ);
	void SingleFile(int frame, std::vector<int>& orderedParticles, std::vector<float>& orderedX, std::vector<float>& orderedY, std::vector<float>& orderedZ);
	void SingleFileDouble(int frame, std::vector<int>& orderedParticles, std::vector<std::pair<float,float>>& orderedX, std::vector<std::pair<float,float>>& orderedY,
		std::vector<std::pair<float,float>>& orderedZ);
	void makeFigureNew(cv::Mat* exibir, circles_data& point, int& label);
	void makeFigureUpdate(cv::Mat* exibir, circles_data& point, circles_data& pastPoint, int& label);
	void updateLabels(cv::Mat* exibir, std::vector<circles_data>& points, std::vector<circles_data>& pastPoints, int* lines_assignment, int lines_size);
	void firstPass(std::vector<circles_data>& points);

	void merge(int* array, int const left, int const mid, int const right, std::vector<int>& particles);
	void mergeSort(int* array, int const begin, int const end, std::vector<int>& particles);
	template <typename T> void orderVectors(std::vector<int>& particles, std::vector<T>& x, std::vector<T>& y, std::vector<T>& z);

	float velocitySquared(circles_data& p1, circles_data& p2);

	void mergeFiles();
};



