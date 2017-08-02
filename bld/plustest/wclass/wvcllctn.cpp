#include "wvcllctn.hpp"

WEXPORT WVCollection::WVCollection()
{
}

WEXPORT WVCollection::~WVCollection()
{
}

WObject* WEXPORT WVCollection::remove( WObject* obj )
{
	ifptr( find( obj ) ) {
		return removeSame( obj );
	}
	return NIL;
}
