#pragma once
#include "defines.hpp"

class RecommenderTrainer 
{
public:
	virtual Train();
	virtual ~RecommenderTrainer();
};
