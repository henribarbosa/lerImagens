#pragma once

#include <cmath>

struct circles_data
{
	float x{};
	float y{};
	float r{};
	bool inPlug{};
	float brightness{};
	int type{};
	circles_data(float _x, float _y, float _r, bool _inPlug, float _brightness, int _type);

	float dist(circles_data c1, circles_data c2);
};

