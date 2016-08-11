#pragma once
#include "defines.hpp"
#include "recommender.hpp"

class RecommenderTrainer 
{
public:
	RecommenderTrainer(Recommender* recommender);
	Recommender* recommender;
	virtual void Train() = 0;
	virtual ~RecommenderTrainer() = 0;
};
