#pragma once

#include <vector>
#include <string>
#include <iostream>

// functions to preprocess:
// average background
// subtract background
// threshold image
// rotate the image to correct angle for processing

void MakeAveragePicture(std::vector<std::string> files);

void SubtractMask(std::vector<std::string> files);

void MakeBinary(std::vector<std::string> files);

void RotateImages(std::vector<std::string> files);


