#ifndef wvcollection_class
#define wvcollection_class

#include "wobject.hpp"

#define FOREACH( c, s, p ) \
	{ for( c* p = (c*)s.first(); p != NIL; p=(c*)s.next(p) ) {
#define FOREACHP( c, s, p ) \
	{ for( c* p = (c*)s->first(); p != NIL; p=(c*)s->next(p) ) {
#define END }}

WCLASS WVCollection : public WObject
{
	public:
		WEXPORT WVCollection();
		WEXPORT ~WVCollection();
		WObject* WEXPORT remove( WObject* obj );
		virtual WObject* WEXPORT find( WObject* obj ) = 0;
		virtual WObject* WEXPORT add( WObject* obj ) = 0;
		virtual WObject* WEXPORT removeSame( WObject* obj ) = 0;
		virtual int WEXPORT count() = 0;
};

#endif
