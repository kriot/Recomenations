#include "data.hpp"

void Data::AddData(UID uid, IID iid, R rating) {
	data.push_back({uid, iid, rating});
}
