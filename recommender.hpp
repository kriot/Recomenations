#pragma once
#include <opencv2/core/core.hpp>
#include "defines.hpp"

class Recommender {
public:
	Recommender(long long UsersSize, long long ItemsSize, long long Dimension);
	cv::Mat Users; // user is row
	cv::Mat Items; // item is column
	int Dimension;
	R Predict(UID uid, IID iid);
};
