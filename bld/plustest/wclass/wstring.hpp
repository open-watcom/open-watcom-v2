#ifndef wstring_class
#define wstring_class

#define strieq(a,b) (stricmp((a),(b))==0)

#include "wobject.hpp"
#include "string.h"

WCLASS WString : public WObject
{
	public:
		WEXPORT WString();
		WEXPORT WString( WString const& );
		WEXPORT WString( char * );
		WEXPORT WString( unsigned int );
		WEXPORT ~WString();
		virtual bool WEXPORT isEqual( WObject* );
		virtual bool WEXPORT operator==( char* cstring );
		char& operator[]( int index ) { return _value[ index ]; }
		WEXPORT operator char*();
		int WEXPORT size() { return (_value==NIL) ? 0 : strlen( _value ); }
	private:
		char*  _value;
};

typedef WString* (WObject::*sbc)( char* c );

#endif //wstring_class

