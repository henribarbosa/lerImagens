#pragma once

struct circles_data
{
	float x{};
	float y{};
	float r{};
	bool inPlug{};
	int type;
	circles_data(float x, float y, float r, bool inPlug, int type);
};

