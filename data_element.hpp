#include "defines.hpp"

struct DataElement {
	UID uid;
	IID iid;
	R rating;
};

struct DataSessionElement {
	ID uid;
	ID session;
	ID iid;
	R rating;
	DataElement interpret_like_user_item_data();
};
