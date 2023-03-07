#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <thread>
#include <future>

struct circles_data
{
	int x{};
	int y{};
	bool inPlug{};
};

struct tripleInt
{
	int first, second, third;
};

class hungarian_fast
{
private:
	std::vector<circles_data> frame0, frame1;
	int connection_lines = 0, connection_columns = 0, connections_size = 0;
	float* connections = nullptr;
	std::vector<std::pair<int, int>> assignment, prohibited;
	std::vector<tripleInt> zeros;
	float maxDist{}, big_value = 1000000;
	float dist(circles_data* p1, circles_data* p2);
	bool read_files();
	void buildConnections();
	void buildProhibiteds(float cut);
	void clearProhibiteds();
	void makeAssignments();
	int findMinimum();
	bool checkAssignment();
	void updateFiles();
	void findPath(int index);
	bool zerosCovered(std::vector<int>* col, std::vector<int>* lin);

public:
	hungarian_fast(float dist);
	virtual ~hungarian_fast();
	void run();
	void runTest();
	void makeFigure(cv::Mat* exibir);
};
