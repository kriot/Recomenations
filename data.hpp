#pragma once
#include <vector>
#include "data_element.hpp"

// analog of TrivialPairDataSet of old_main.cpp
class Data {
public:
	std::vector<DataElement> Data;
	void AddData(UID uid, IID iid, R rating);
};
