#pragma once
#include <opencv2/core/core.hpp>
#include "defines.hpp"

class Recommender {
public:
	Recommender(long long UsersSize, long long ItemsSize, long long Dimension);
	int Dimension;
	cv::Mat Users; // user is row
	cv::Mat Items; // item is column
	R Predict(UID uid, IID iid);
};
