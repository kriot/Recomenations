#pragma once
#include <chrono>
#include "recommender.hpp"
#include "data.hpp"
#include "recommender_trainer.hpp"

class RecommenderSessionNormalAroundUser: public RecommenderTrainer {
	public:
		RecommenderSessionNormalAroundUser(
				Recommender* recommender
				, DataSession* data
				, int timeup
				, double lambda
				, double regularization_k
				, long long N = 1000);
		~RecommenderSessionNormalAroundUser();

		void Train() override; 

		double RMSE(); 

	private:
		inline double RMSE_partial(int offset); 
		double MSE_sessions();
		double UserSession_metric();
	public:
		std::chrono::milliseconds timeup;
		long long N = 1000;
		int done_iterations = 0;
		double lambda;
		double regularization_k;
	private:
		DataSession* data;
		cv::Mat sessions;
		double mu;
	private:
		void EstimateMu(); 
		// User: (mu,   1, b_u, ... )
		// ItemL ( 1, b_i,   1, ... )
		inline void FixUser(UID uid); 
		inline void FixItem(IID iid); 
		inline void FixSession(ID session);
		inline void FixAll(); 
		void RandomizeData(); 
};
