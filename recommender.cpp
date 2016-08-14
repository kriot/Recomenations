#include "recommender.hpp"

Recommender::Recommender(long long UsersSize, long long ItemsSize, long long Dimension)
	: Users(cv::Mat(UsersSize, Dimension, CV_R))
	, Items(cv::Mat(Dimension, ItemsSize, CV_R))
	, Dimension(Dimension)
{
}

R Recommender::Predict(UID uid, IID iid)
{
	return cv::Mat(Users.row(uid) * Items.col(iid)).at<R>(0, 0);
}
