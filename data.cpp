#include "data.hpp"

void Data::AddData(UID uid, IID iid, R rating) {
	data.push_back({uid, iid, rating});
}

void DataSession::AddData(UID uid, ID session, IID iid, R rating) {
	data.push_back({uid, session, iid, rating});
}
