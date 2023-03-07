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
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

#include "lagrangian.h"
#include "circles_data.h"
#include "read_control.h"

class auction
{
private:
	std::vector<circles_data> frame0, frame1;
	std::vector<std::pair<int, int>> prohibited;
	int connections_lines = 0, connections_columns = 0, connections_size = 0;
	float* connections = nullptr;
	std::deque<int> lines_to_do;
	int* lines_assignment = nullptr;
	int* columns_assignment = nullptr;
	float* prices = nullptr;
	float maxDist = 0, big_value = 100000, epsilon = 0;

	float dist(circles_data* p1, circles_data* p2);
	bool read_files();
	void buildConnections();
	void buildProhibiteds();
	void buildProhibitedsOutlier();
	void clearProhibiteds();
	void clearMemory();
	void minimumFunction(float* min, float* second_min, int* min_index, int* index);
	int findMinimum();
	void updateFiles();
	bool testEnd();

public:
	auction(float dist);
	virtual ~auction();
	void run(cv::Mat* exibir, lagrangian& particle_track);
	void runTest();
	void makeFigure(cv::Mat* exibir);
};
