#pragma once
#include "opencv2/opencv.hpp"

cv::Mat MakeRegionsMap( const cv::Mat & verticalAdjacency, const cv::Mat & horizontalAdjacency, int minSize = 1 );
// TO DO: implement small regions merging algorithm