#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "matrix.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <thread>
#include <future>

class hungarian
{
public:
	matrix frame0, frame1;
	matrix connections;
	matrix assignment, prohibted;
	double max_dist, big_value = 10000;

	hungarian (double dist);
	virtual ~hungarian ();
	double dist(matrix p1, matrix p2);
	bool read_files();
	void build_connections();
	void build_prohibiteds(double cut); 
	void clear_prohibiteds(matrix& zeros);
	void make_assignments(matrix& zeros); 
	int find_minimum();
	bool check_assignment(matrix& zeros); 
	void show_connections(); 
	void atualiza_arquivos();
	void run();
	void find_path(int index, matrix& zeros);
	void make_figure(const char* path);
	void run_test();
};
