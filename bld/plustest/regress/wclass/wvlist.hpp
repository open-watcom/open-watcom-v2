#ifndef wvlist_class
#define wvlist_class

#include "wvcllctn.hpp"

WCLASS WVList : public WVCollection
{
	public:
		WEXPORT WVList();
		WEXPORT WVList( WVList& );
		WEXPORT ~WVList();
		int WEXPORT count() { return _free; }
		WObject* WEXPORT find( WObject *obj );
		WObject* WEXPORT add( WObject* obj );
		int WEXPORT indexOfSame( WObject *obj );
		WObject* WEXPORT removeAt( int index );
		WObject* WEXPORT removeSame( WObject* obj );
		WObject* WEXPORT first();
		WObject* WEXPORT last();
		WObject* WEXPORT next( WObject* obj );
		WObject* WEXPORT prev( WObject* obj );
	protected:
		WObject** _set;
		int     _count;
		int     _free;
};

#endif //wvlist_class

