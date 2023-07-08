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

// class to solve the assignment problem in a square matrix
// Method focused in performance, may not find the optimum solution, but for our porpoise is good enough

class auction
{
private:
	std::vector<circles_data> frame0, frame1; // the frames for tracking, find the particles identified in 0 at the time 1
	std::vector<std::pair<int, int>> prohibited; // particles too far, disconsider and accept that the particles were lost
	int connections_lines = 0, connections_columns = 0, connections_size = 0; // size of the problem
	float* connections = nullptr; // pointer to store the connections
	std::deque<int> lines_to_do; // stack to run the algorithm
	int* lines_assignment = nullptr; // connections in terms of the lines
	int* columns_assignment = nullptr; // the same in terms of columns
	float* prices = nullptr; // prices to run the algorithms
	float maxDist = 0, big_value = 100000, epsilon = 0; // auxiliary variables to run

	float dist(circles_data* p1, circles_data* p2); // distance between particles
	bool read_files(); // open files of the particles positions
	void buildConnections(); // fill the problem's matrix
	void buildProhibiteds(); // build list of particles too far
	void buildProhibitedsOutlier(); // build the list of particles going in the wrong direction
	void clearProhibiteds(); // remove prohibiteds and accept the losses
	void clearMemory(); // free memory after calculations
	void minimumFunction(float* min, float* second_min, int* min_index, int* index); // find minimum
	int findMinimum();
	void updateFiles();
	bool testEnd(); // stop the run at a good enough solution

public:
	auction(float dist); // calls the algorithm
	virtual ~auction(); // virtual destructor
	void run(cv::Mat* exibir, lagrangian& particle_track); // runs the tracking
	void runTest(); // used to test the method
	void makeFigure(cv::Mat* exibir); // visual aid
};
