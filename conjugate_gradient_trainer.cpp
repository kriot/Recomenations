#include "conjugate_gradient_trainer.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <iostream>
#include <fstream>

RecommenderConjugateGradient::RecommenderConjugateGradient(
		Recommender* recommender
		, Data* data
		, int timeup
		, double lambda
		, double regularization_k
		, long long N)
	: RecommenderTrainer(recommender)
	, timeup(timeup) 
	, N(N)
	, lambda(lambda) 
	, regularization_k(regularization_k)
	, data(data)
{
}

RecommenderConjugateGradient::~RecommenderConjugateGradient()
{
}

void RecommenderConjugateGradient::Train() {
//
// Artcle of method: https://en.wikipedia.org/wiki/Nonlinear_conjugate_gradient_method
//
	RandomizeData();
	EstimateMu();
	FixAll();
	double rmse = RMSE();
	std::cerr << "First RMSE: " << rmse << "\n";
	std::ofstream lambda_log("lambda.log");
	std::ofstream rmse_log("rmse.log");
	std::ofstream speed_log("speed.log");
	std::ofstream acceleration_log("acceleration.log");
	auto beginning = std::chrono::system_clock::now();

	const int U = recommender->Users.rows;
	const int M = recommender->Items.cols;
	const int dim = recommender->Dimension;

	cv::Mat user_delta_x_prev(U, dim, CV_R);
	cv::Mat movie_delta_x_prev(dim, M, CV_R);
	cv::Mat u0 = user_delta_x_prev.clone();
	cv::Mat m0 = movie_delta_x_prev.clone();
	for (int iteration = 0; iteration < N && std::chrono::system_clock::now() - beginning < timeup && rmse > 0.1; ++iteration) {
		std::cerr << "Iteration: " << iteration << "\n";
		if (iteration % 10 == 0) {
			std::cerr << "Iteration: " << iteration << "\n";
			rmse = RMSE();
			std::cerr << "RMSE: " << rmse << "\n";
		}

		if (iteration % 2 == 0) { // # train u_id
			cv::Mat user_delta_x = cv::Mat::zeros(U, dim, CV_R);
			// estimate gradient
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.rating;
				user_delta_x.row(u) += recommender->Items.col(m).t() * (r - recommender->Predict(u, m));
			}
			// wtf ?
			for(int u = 0; u < U; ++u) {
				user_delta_x.at<R>(u, 0) = 0;
				user_delta_x.at<R>(u, 1) = 0;
			}

			// estimate number for the method (see article)
			double beta_pr = iteration < 2 ? 0 : user_delta_x.dot(user_delta_x - user_delta_x_prev) / 
													user_delta_x_prev.dot(user_delta_x_prev);
			beta_pr = 0;
			double beta = std::max(0., beta_pr);

			// Update the conjugate direction (see article, step 3)
			u0 = user_delta_x + (u0 * beta);

			// optimize (step 4)
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.rating;
				t_num += cv::Mat((r - recommender->Predict(u, m)) * (u0.row(u) * recommender->Items.col(m))).at<R>(0, 0);
				t_den += std::pow(cv::Mat(u0.row(u) * recommender->Items.col(m)).at<R>(0, 0), 2); 
			}
			double t = t_num / t_den;
			
			// apply (step 5)
			recommender->Users += u0 * t;
			
			// save prev
			user_delta_x_prev = user_delta_x;
		} else { // # train m_id
			cv::Mat movie_delta_x = cv::Mat::zeros(dim, M, CV_R);
			// estimate gradient
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.rating;
				movie_delta_x.col(m) += recommender->Users.row(u).t() * (r - recommender->Predict(u, m));
			}

			// wtf ?
			for(int m = 0; m < M; ++m) {
				movie_delta_x.at<R>(0, m) = 0;
				movie_delta_x.at<R>(2, m) = 0;
			}

			// estimate number for the method (see article)
			double beta_pr = iteration < 2 ? 0 : movie_delta_x.dot(movie_delta_x - movie_delta_x_prev) / 
													movie_delta_x_prev.dot(movie_delta_x_prev);
			beta_pr = 0;
			double beta = std::max(0., beta_pr);

			// Update the conjugate direction (see article, step 3)
			m0 = movie_delta_x + m0 * beta;

			// optimize (step 4)
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.rating;
				t_num += cv::Mat((r - recommender->Predict(u, m)) * (recommender->Users.row(u) * m0.col(m))).at<R>(0, 0);
				t_den += std::pow(cv::Mat(recommender->Users.row(u) * m0.col(m)).at<R>(0, 0), 2); 
			}
			double t = t_num / t_den;

			// apply (step 5)
			recommender->Items += m0 * t;

			// save prev
			movie_delta_x_prev = movie_delta_x;
		}
		// FixAll();
		done_iterations = iteration + 1;
	}
}

double RecommenderConjugateGradient::RMSE() {
	double sum = 0;
	for (const auto& d: data->data) {
		int u = d.uid;
		int m = d.iid;
		int r = d.rating;
		double error = r - recommender->Predict(u, m);
		sum += std::pow(error, 2);
	}
	return sum / data->data.size();
}

void RecommenderConjugateGradient::EstimateMu() {
	double sum = 0;
	for (const auto& d: data->data) {
		int r = d.rating;
		sum += r; 
	}
	mu = sum / data->data.size();
	//		std::cerr << "Mu: " << mu << "\n";
}
// User: (mu,   1, b_u, ... )
// ItemL ( 1, b_i,   1, ... )
inline void RecommenderConjugateGradient::FixUser(UID uid) {
	recommender->Users.at<R>(uid, 0) = mu;
	recommender->Users.at<R>(uid, 1) = 1;
}

inline void RecommenderConjugateGradient::FixItem(IID iid) {
	recommender->Items.at<R>(0, iid) = 1.;
	recommender->Items.at<R>(2, iid) = 1.;
}

inline void RecommenderConjugateGradient::FixAll() {
	for (int i = 0; i < recommender->Users.rows; ++i)
		FixUser(i);
	for (int i = 0; i < recommender->Items.cols; ++i)
		FixItem(i);
}

void RecommenderConjugateGradient::RandomizeData() {
	cv::randn(recommender->Users, 0, 1);
	cv::randn(recommender->Items, 0, 1);
}
