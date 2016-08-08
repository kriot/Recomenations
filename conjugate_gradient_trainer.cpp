#include "conjugate_gradient_trainer.hpp"

RecommenderConjugateGradient::RecommenderConjugateGradient(
		Recommender* recommender
		, Data* data
		, int timeup
		, double lambda
		, double regularization_k
		, long long N = 1000)
	: SVDRecommenderTrainerInterface(recommender)
	, timeup(timeup) 
	, N(N)
	, lambda(lambda) 
	, regularization_k(regularization_k)
	, data(data)
	  , shuffle(data->data.size())
{
}

void RecommenderConjugateGradient::Train() override {
	RandomizeData();
	EstimateMu();
	FixAll();
	int rmse_offset = 0;
	double rmse = RMSE();
	double speed = -1.0;
	double acceleration = 0.0;
	double last_lambda = 0;
	std::cerr << "First RMSE: " << rmse << "\n";
	std::ofstream lambda_log("lambda.log");
	std::ofstream rmse_log("rmse.log");
	std::ofstream speed_log("speed.log");
	std::ofstream acceleration_log("acceleration.log");
	long long log_id = 0;
	auto beginning = std::chrono::system_clock::now();

	const int U = recommender->users.size();
	const int M = recommender->items.size();
	const int dim = recommender->users[0].size();

	Mat user_delta_x_prev(U, MathVect<>(dim));
	Mat movie_delta_x_prev(M, MathVect<>(dim));
	Mat u0 = user_delta_x_prev;
	Mat m0 = movie_delta_x_prev;
	for (int iteration = 0; iteration < N && std::chrono::system_clock::now() - beginning < timeup && rmse > 0.1; ++iteration) {
		if (iteration % 10 == 0) {
			std::cerr << "Iteration: " << iteration << "\n";
			rmse = RMSE();
			std::cerr << "RMSE: " << rmse << "\n";
		}

		if (iteration % 2 == 0) { // # train u_id
			Mat user_delta_x(U, MathVect<>(dim));
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.meta;
				user_delta_x[u] += recommender->items[m] * (r - recommender->Predict(u, m));
			}
			for(int u = 0; u < U; ++u) {
				user_delta_x[u][0] = 0;
				user_delta_x[u][1] = 0;
			}
			double beta_pr = iteration < 2 ? 0 : (user_delta_x * (user_delta_x - user_delta_x_prev)) / (user_delta_x_prev * user_delta_x_prev);
			double beta = std::max(0., beta_pr);
			u0 = user_delta_x + (u0 * beta);
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.meta;
				t_num += (r - recommender->Predict(u, m)) * (u0[u] * recommender->items[m]);
				t_den += std::pow(u0[u] * recommender->items[m], 2); 
			}
			double t = t_num / t_den;
			for(int u = 0; u < U; ++u) {
				recommender->users[u] += u0[u] * t;
			}

			user_delta_x_prev = user_delta_x;
		} else { // # train m_id
			Mat movie_delta_x(M, MathVect<>(dim));
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.meta;
				movie_delta_x[m] += recommender->users[u] * (r - recommender->Predict(u, m));
			}
			for(int m = 0; m < M; ++m) {
				movie_delta_x[m][0] = 0;
				movie_delta_x[m][2] = 0;
			}
			double beta_pr = iteration < 2 ? 0 : (movie_delta_x * (movie_delta_x - movie_delta_x_prev)) / (movie_delta_x_prev * movie_delta_x_prev);
			double beta = std::max(0., beta_pr);
			m0 = movie_delta_x + m0 * beta;
			double t_num = 0;
			double t_den = 0;
			for (const auto& d: data->data) {
				int u = d.uid;
				int m = d.iid;
				int r = d.meta;
				t_num += (r - recommender->Predict(u, m)) * (m0[m] * recommender->users[u]);
				t_den += std::pow(m0[m] * recommender->users[u], 2); 
			}
			double t = t_num / t_den;
			for (int m = 0; m < M; ++m) {
				recommender->items[m] += m0[m] * t;
			}

			movie_delta_x_prev = movie_delta_x;
		}
		// FixAll();
		done_iterations = iteration + 1;
	}
}

double RecommenderConjugateGradient::RMSE() override {
	double sum = 0;
	for (const auto& d: data->data) {
		int u = d.uid;
		int m = d.iid;
		int r = d.meta;
		double error = r - recommender->Predict(u, m);
		sum += std::pow(error, 2);
	}
	return sum / data->data.size();
}

inline double RecommenderConjugateGradient::RMSE_partial(int offset) {
	double sum = 0;
	int size = data->data.size();
	int part_size = std::min (10000, size);
	for (int i = 0; i < part_size; ++i) {
		auto const& d = data->data[shuffle((i + offset) % data->data.size())];	
		int u = d.uid;
		int m = d.iid;
		int r = d.meta;
		sum += std::pow(r - recommender->Predict(u, m), 2);
	}
	return sum / part_size;
}

void RecommenderConjugateGradient::EstimateMu() {
	double sum = 0;
	for (const auto& d: data->data) {
		int r = d.meta;
		sum += r; 
	}
	mu = sum / data->data.size();
	//		std::cerr << "Mu: " << mu << "\n";
}
// User: (mu,   1, b_u, ... )
// ItemL ( 1, b_i,   1, ... )
inline void RecommenderConjugateGradient::FixUser(UID uid) {
	auto& user = recommender->users[uid];
	user[0] = mu;
	user[1] = 1.;
}

inline void RecommenderConjugateGradient::FixItem(IID iid) {
	auto& item = recommender->items[iid];
	item[0] = 1.;
	item[2] = 1.;
}

inline void RecommenderConjugateGradient::FixAll() {
	for (int i = 0; i < recommender->users.size(); ++i)
		FixUser(i);
	for (int i = 0; i < recommender->items.size(); ++i)
		FixItem(i);
}

void RecommenderConjugateGradient::RandomizeData() {
	for (auto& user: recommender->users)
		user = RandomVector(user.size());
	for (auto& item: recommender->items)
		item = RandomVector(item.size());
}
