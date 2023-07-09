#include "../include/circles_data.h"

// structure to store particles data

circles_data::circles_data(float _x, float _y, float _r, bool _inPlug, float _brightness, int _type)
	:x(_x), y(_y), r(_r), inPlug(_inPlug), brightness(_brightness), type(_type)
{}

float circles_data::dist(circles_data c1, circles_data c2)
{
	float dx2 = pow(c2.x - c1.x,2);
	float dy2 = pow(c2.y - c1.y,2);
	return sqrt(dx2+dy2);
}
