#pragma once

#define _USE_MATH_DEFINES
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

// save numerical parameters

struct parameter
{
    float threshold{};
    std::string name{};
};

// read control (thresholds) file for numerical parameters

class control
{
private:
    parameter Param;

public:
    control(const std::string& path, const std::string& fieldName); // find the correct parameter
    ~control();
    float returnThreshold(); // returns the numerical value
};
