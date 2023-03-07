#pragma once

#define _USE_MATH_DEFINES
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

struct parameter
{
    float threshold{};
    std::string name{};
};

class control
{
private:
    parameter Param;

public:
    control(const std::string& path, const std::string& fieldName);
    ~control();
    float returnThreshold();
};
