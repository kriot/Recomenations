#include <iostream>
#include "data.hpp"
#include "recommender.hpp"
#include "conjugate_gradient_trainer.hpp"


// TODO: opt_parse
int main() {

	bool model_choosing = true;
	bool testing = false;

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
			if (rmse < eps) { // overlearning
				optimal_size = std::max(3, i - 1);
				break;
			}
		}
	}
	std::cerr << "optimal_size: " << optimal_size << "\n";

	SVDRecommender recommender(U, M, optimal_size);
	SVDRecommenderConjugateGradient trainer(&recommender, &dataset
		, 50000 /*timeup*/
		, 0.002 /*lambda*/
		, 0.0 /*regularization*/
		, 10000 * D);
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