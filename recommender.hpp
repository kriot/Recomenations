#pragma once
#include <opencv2/core/core.hpp>
#include "defines.hpp"

class Recommender {
public:
	int Dimesion;
	cv::Mat Users; // user is row
	cv::Mat Items; // item is column
	R Prdict(UID uid, IID iid);
};
