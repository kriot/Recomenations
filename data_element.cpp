#include "data_element.hpp"
DataElement DataSessionElement::interpret_like_user_item_data()
{
	return DataElement{session, iid, rating};
}
