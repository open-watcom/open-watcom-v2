#ifndef wobjectmap_class
#define wobjectmap_class

#include "wvlist.hpp"
#define HANDLE unsigned

WCLASS WObjectMap : public WVList
{
	public:
		WEXPORT WObjectMap() : _currThis( NIL ) {}
		WEXPORT ~WObjectMap() {}

		void WEXPORT setThis( WObject* obj, HANDLE hand );
		void WEXPORT clearThis( WObject* obj );
		WObject* WEXPORT findThis( HANDLE hand );
		WObject* WEXPORT currThis() { return _currThis; }
		void WEXPORT currThis( WObject* obj ) { _currThis = obj; }
	private:
		WObject*	_currThis;
};

#endif


