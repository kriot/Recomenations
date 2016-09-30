#include <iostream>
#include "data.hpp"
#include "recommender.hpp"
#include "conjugate_gradient_trainer.hpp"
#include "recommender_session_normal_around_user.hpp"


// TODO: opt_parse
void run_user_item_model()
{
	bool model_choosing = false;

	long long k, U, M, D, T;
	std::cin >> k >> U >> M >> D >> T;
	
	Data dataset;
	for (int i = 0; i < D; ++i) {
		int u, m;
		double r;
		std::cin >> u >> m >> r;
		dataset.AddData(u, m, r);
	}

	int optimal_size = 10; // Maximal size
	if (model_choosing) {
		for (const auto& i: std::vector<int>{3,10}) { // search the last non-overlearned model
			std::cerr << "Trying n = " << i << "\n";
			Recommender recommender(U, M, i);
			//		SVDRecommenderTrainerStochasticGradientWithMuAndBias trainer(&recommender, &dataset);
			//		SVDRecommenderOnlineTrainer trainer(&recommender, std::cin);
			RecommenderConjugateGradient trainer(&recommender, &dataset
					, 2000 /*timeup*/
					, 0.001 /*lambda*/
					, 0.1 /*regularization*/
					, 10000 * D /*max iterations*/);
			trainer.Train();
			double rmse = trainer.RMSE();
			std::cerr << "RMSE: " << rmse << "\n";
			std::cerr << "done_iterations: " << trainer.done_iterations << "\n";
			if (rmse < 0.001) { // overlearning
				optimal_size = std::max(3, i - 1);
				break;
			}
		}
	}
	std::cerr << "optimal_size: " << optimal_size << "\n";

	Recommender recommender(U, M, optimal_size);
	RecommenderConjugateGradient trainer(&recommender, &dataset
		, 100 * 60 * 1000 /*timeup*/
		, 0.0002 /*lambda*/
		, .01 /*regularization*/
		, 30);
	trainer.Train();
#ifdef DEBUG
	double rmse = trainer.RMSE();
	std::cerr << "RMSE: " << rmse << "\n";
	std::cerr << "done_iterations: " << trainer.done_iterations << "\n";
#endif

//	std::cout << "Answer: \n";
	for (int i = 0; i < T; ++i) {
		int u, m;
		std::cin >> u >> m;
//		std::cout << "U: " << u << "; M: " << m << "\n";
		std::cout << recommender.Predict(u, m) << "\n";
//		std::cout << recommender.users[u]  << "; " << recommender.items[m] << "\n";
	}
}

void run_user_session_item_model()
{
	long long UsersSize, SessionsSize, ItemsSize, TrainSize, TestSize;
	std::cin >> UsersSize >> SessionsSize >> ItemsSize >> TrainSize >> TestSize;
	
	DataSession dataset(UsersSize, SessionsSize, ItemsSize);
	for (int i = 0; i < TrainSize; ++i) {
		int u, m, s;
		double r;
		std::cin >> u >> s >> m >> r;
		dataset.AddData(u, s, m, r);
	}

	int optimal_size = 10; // Maximal size
	std::cerr << "optimal_size: " << optimal_size << "\n";

	Recommender recommender(UsersSize, ItemsSize, optimal_size);
	RecommenderSessionNormalAroundUser trainer(&recommender, &dataset
		, 100 * 60 * 1000 /*timeup*/
		, 0.0002 /*lambda*/
		, .01 /*regularization*/
		, 30);
	trainer.Train();
#ifdef DEBUG
	double rmse = trainer.RMSE();
	std::cerr << "RMSE: " << rmse << "\n";
	std::cerr << "done_iterations: " << trainer.done_iterations << "\n";
#endif

//	std::cout << "Answer: \n";
	for (int i = 0; i < TestSize; ++i) {
		int u, m;
		std::cin >> u >> m;
//		std::cout << "U: " << u << "; M: " << m << "\n";
		std::cout << recommender.Predict(u, m) << "\n";
//		std::cout << recommender.users[u]  << "; " << recommender.items[m] << "\n";
	}
}

int main() {
	run_user_item_model();
	// run_user_session_item_model();
}
