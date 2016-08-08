#pragma once
#include "recommender.hpp"
#include "data.hpp"
#include "recommender_trainer.hpp"

class RecommenderConjugateGradient: public RecommenderTrainer {
	public:
		RecommenderConjugateGradient(
				Recommender* recommender
				, Data* data
				, int timeup
				, double lambda
				, double regularization_k
				, long long N = 1000);

		void Train() override; 

		double RMSE() override; 

	private:
		inline double RMSE_partial(int offset); 
	public:
		std::chrono::milliseconds timeup;
		long long N = 1000;
		int done_iterations = 0;
		double lambda;
		double regularization_k;
	private:
		Data* data;
		Shuffler shuffle;	
		double mu;
	private:
		void EstimateMu(); 
		// User: (mu,   1, b_u, ... )
		// ItemL ( 1, b_i,   1, ... )
		inline void FixUser(UID uid); 
		inline void FixItem(IID iid); 
		inline void FixAll(); 
		void RandomizeData(); 
};
