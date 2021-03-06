#include "recommender_session_normal_around_user.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <iostream>
#include <fstream>

RecommenderSessionNormalAroundUser::RecommenderSessionNormalAroundUser(
		Recommender* recommender
		, DataSession* data
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

RecommenderSessionNormalAroundUser::~RecommenderSessionNormalAroundUser()
{
}

void RecommenderSessionNormalAroundUser::Train() {
//
// Artcle of method: https://en.wikipedia.org/wiki/Nonlinear_conjugate_gradient_method
//
	std::ofstream lambda_log("lambda.log");
	std::ofstream rmse_log("rmse.log");
	std::ofstream speed_log("speed.log");
	std::ofstream acceleration_log("acceleration.log");
	auto beginning = std::chrono::system_clock::now();

	const int U = recommender->Users.rows;
	const int M = recommender->Items.cols;
	const int dim = recommender->Dimension;
	const int S = data->sessions;

	int steps_per_iteration = 1;
	bool disable_conjugate = false;

	cv::Mat session_delta_x_prev = cv::Mat::zeros(S, dim, CV_R);
	sessions = cv::Mat::zeros(S, dim, CV_R);
	cv::randn(sessions, 0, 1);

	cv::Mat movie_delta_x_prev = cv::Mat::zeros(dim, M, CV_R);
	cv::Mat u0 = session_delta_x_prev.clone();
	cv::Mat m0 = movie_delta_x_prev.clone();

	RandomizeData();
	EstimateMu();
	FixAll();
	double rmse = RMSE();
	std::cerr << "First RMSE: " << rmse << "\n";

	for (int iteration = 0; iteration < N && std::chrono::system_clock::now() - beginning < timeup; ++iteration) {
		std::cerr << "Iteration: " << iteration << "\n";
		if (iteration % 1 == 0) {
			std::cerr << "Iteration: " << iteration << "\n";
			rmse = RMSE();
			std::cerr << "MSE: " << rmse << "\n";
			double mse_sessions = MSE_sessions();
			std::cerr << "MSE sessions: " << mse_sessions << "\n";
		}

		if ((iteration / steps_per_iteration) % 2 == 0) { // # train s_id

			// reset m0 after all iterations for mids
			m0 = cv::Mat::zeros(dim, M, CV_R);

			cv::Mat session_delta_x = cv::Mat::zeros(S, dim, CV_R);
			// estimate gradient
			for (const auto& d: data->data) {
				// ID u = d.uid;
				ID s = d.session;
				ID m = d.iid;
				double r = d.rating;
				session_delta_x.row(s) += recommender->Items.col(m).t() * (r - sessions.row(s).dot(recommender->Items.col(m).t())) + regularization_k * (recommender->Users.row(s) + sessions.row(s));
			}
			// reset
			for(int s = 0; s < S; ++s) {
				session_delta_x.at<R>(s, 0) = 0;
				session_delta_x.at<R>(s, 1) = 0;
			}

			// estimate number for the method (see article)
			double beta_pr = iteration < 2 ? 0 : session_delta_x.dot(session_delta_x - session_delta_x_prev) / 
													session_delta_x_prev.dot(session_delta_x_prev);
			beta_pr = 0;
			double beta = std::max(0., beta_pr);
			
			if (disable_conjugate)
				beta = 0;

			// Update the conjugate direction (see article, step 3)
			u0 = session_delta_x + (u0 * beta);

			// optimize (step 4)
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				// int u = d.uid;
				int m = d.iid;
				ID s = d.session;
				int r = d.rating;
				t_num += cv::Mat((r - sessions.row(s).dot(recommender->Items.col(m).t())) * (u0.row(s) * recommender->Items.col(m))).at<R>(0, 0) + regularization_k * cv::Mat((recommender->Users.row(s) + sessions.row(s)) * (u0.row(s).t())).at<R>(0, 0);
				t_den += std::pow(cv::Mat(u0.row(s) * recommender->Items.col(m)).at<R>(0, 0), 2) + regularization_k * cv::Mat(u0.row(s) * (u0.row(s).t())).at<R>(0, 0); 
			}
			double t = t_num / t_den;
			
			// apply (step 5)
			sessions += u0 * t;
			
			// save prev
			session_delta_x_prev = session_delta_x;
			
			//fit users
			std::vector<int> n(U, 0);
			recommender->Users = cv::Mat::zeros(U, dim, CV_R);
			for (const auto& d: data->data) {
				ID u = d.uid;
				ID s = d.session;
				// ID m = d.iid;
				recommender->Users.row(u) += sessions.row(s);
				n[u] += 1;
			}
			for (const auto& d: data->data) {
				ID u = d.uid;
				// ID s = d.session;
				// ID m = d.iid;
				recommender->Users.row(u) /= n[u];
				n[u] = 1;
			}
			std::cerr << "After user opt: " << MSE_sessions() << "\n";
			std::cerr << "UserSession metric: " << UserSession_metric() << "\n";
		} else { // # train m_id

			// reset u0 after all iterations for uids
			u0 = cv::Mat::zeros(S, dim, CV_R);
			cv::Mat movie_delta_x = cv::Mat::zeros(dim, M, CV_R);
			// estimate gradient
			for (const auto& d: data->data) {
				// ID u = d.uid;
				ID s = d.session;
				ID m = d.iid;
				double r = d.rating;
				movie_delta_x.col(m) += sessions.row(s).t() * (r - sessions.row(s).dot(recommender->Items.col(m).t())) + regularization_k * recommender->Items.col(m);
			}

			// fixing for byes for items and common byes
			for(int m = 0; m < M; ++m) {
				movie_delta_x.at<R>(0, m) = 0;
				movie_delta_x.at<R>(2, m) = 0;
			}

			// estimate number for the method (see article)
			double beta_pr = iteration < 2 ? 0 : movie_delta_x.dot(movie_delta_x - movie_delta_x_prev) / 
													movie_delta_x_prev.dot(movie_delta_x_prev);
			beta_pr = 0;
			double beta = std::max(0., beta_pr);

			if (disable_conjugate)
				beta = 0;

			// Update the conjugate direction (see article, step 3)
			m0 = movie_delta_x + m0 * beta;

			// optimize (step 4)
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				// int u = d.uid;
				ID s = d.session;
				int m = d.iid;
				int r = d.rating;
				t_num += cv::Mat((r - sessions.row(s).dot(recommender->Items.col(m).t())) * (sessions.row(s) * m0.col(m))).at<R>(0, 0) + regularization_k * cv::Mat(recommender->Items.col(m).t() * m0.col(m)).at<R>(0, 0);
				t_den += std::pow(cv::Mat(sessions.row(s) * m0.col(m)).at<R>(0, 0), 2) + regularization_k * cv::Mat(m0.col(m).t() * m0.col(m)).at<R>(0, 0); 
			}
			double t = t_num / t_den;

			// apply (step 5)
			recommender->Items += m0 * t;

			// save prev
			movie_delta_x_prev = movie_delta_x;
			// std::cerr << "After item opt: " << MSE_sessions() << "\n";
		}
		// FixAll();
		done_iterations = iteration + 1;
	}
//std::cerr << "Time: " << static_cast<int>(std::chrono::system_clock::now() - beginning) / 1000 << " sec.";
}

double RecommenderSessionNormalAroundUser::RMSE() {
	double sum = 0;
	for (const auto& d: data->data) {
		int u = d.uid;
		int m = d.iid;
		double r = d.rating;
		double error = r - recommender->Predict(u, m);
		sum += std::pow(error, 2);
	}
	return sum / data->data.size();
}

double RecommenderSessionNormalAroundUser::MSE_sessions() {
	double sum = 0;
	for (const auto& d: data->data) {
		// int u = d.uid;
		ID m = d.iid;
		ID s = d.session;
		double r = d.rating;
		double error = r - sessions.row(s).dot(recommender->Items.col(m).t());
		sum += std::pow(error, 2);
	}
	return sum / data->data.size();
}

double RecommenderSessionNormalAroundUser::UserSession_metric() {
	double sum = 0;
	std::map<std::pair<ID, ID>, int> user_session_counter;
	std::map<std::pair<ID, ID>, double> user_session_aggregator;
	for (const auto& d: data->data) {
		ID u = d.uid;
		ID s = d.session;
		auto diff = sessions.row(s) - recommender->Users.row(u);
		double error = diff.dot(diff);
		if (user_session_aggregator.find(std::make_pair(u, s)) == user_session_aggregator.end())
			user_session_aggregator[std::make_pair(u, s)] = 0.;
		user_session_aggregator[std::make_pair(u, s)] += error;
		if (user_session_counter.find(std::make_pair(u, s)) == user_session_counter.end())
			user_session_counter[std::make_pair(u, s)] = 0;
		user_session_counter[std::make_pair(u, s)] += 1;
	}
	for (const auto& us: user_session_counter) {
		ID u = us.first.first;
		ID s = us.first.second;
		int c = us.second;
		sum += user_session_aggregator[std::make_pair(u, s)] / c;	
	}
	return sum;
}

void RecommenderSessionNormalAroundUser::EstimateMu() {
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
inline void RecommenderSessionNormalAroundUser::FixUser(UID uid) {
	recommender->Users.at<R>(uid, 0) = mu;
	recommender->Users.at<R>(uid, 1) = 1;
}

inline void RecommenderSessionNormalAroundUser::FixSession(ID session) {
	sessions.at<R>(session, 0) = mu;
	sessions.at<R>(session, 1) = 1;
}

inline void RecommenderSessionNormalAroundUser::FixItem(IID iid) {
	recommender->Items.at<R>(0, iid) = 1.;
	recommender->Items.at<R>(2, iid) = 1.;
}

inline void RecommenderSessionNormalAroundUser::FixAll() {
	for (int i = 0; i < recommender->Users.rows; ++i)
		FixUser(i);
	for (int i = 0; i < recommender->Items.cols; ++i)
		FixItem(i);
	for (int i = 0; i < sessions.rows; ++i)
		FixSession(i);
}

void RecommenderSessionNormalAroundUser::RandomizeData() {
	cv::randn(recommender->Users, 0, 1);
	cv::randn(recommender->Items, 0, 1);
}
