#pragma once
#include <vector>
#include "data_element.hpp"

// analog of TrivialPairDataSet of old_main.cpp
class Data {
public:
	std::vector<DataElement> data;
	void AddData(UID uid, IID iid, R rating);
};

class DataSession {
public:
	DataSession(long long users, long long sessions, long long items);
	std::vector<DataSessionElement> data;
	void AddData(UID uid, ID session, IID iid, R rating);

	long long users;
	long long items;
	long long sessions;
};
