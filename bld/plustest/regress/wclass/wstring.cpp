#include "wstring.hpp"

WEXPORT WString::WString()
	: _value( NIL )
{
}

WEXPORT WString::WString( WString const & x )
	: _value( NIL )
{
	ifptr( x._value ) {
		_value = new char[ strlen( x._value ) + 1 ];
		strcpy( _value, x._value );
	}
}

WEXPORT WString::WString( char* str )
{
	ifptr( str ) {
		ifptr( _value = new char[ strlen( str ) + 1 ] ) {
			strcpy( _value, str );
			return;
		}
		//out or memory error
	}
	_value = NIL;
}

WEXPORT WString::WString( unsigned int len )
{
	ifptr( _value = new char[ len + 1 ] ) {
		return;
	}
	//out or memory error
	_value = NIL;
}

WEXPORT WString::~WString()
{
	ifptr( _value ) {
		delete _value;
	}
}

bool WEXPORT WString::isEqual( WObject* str )
{
	ifptr( str ) { //and assuming str points to a String
		return strcmp( _value, ((WString*)str)->_value ) == 0;
	}
	return FALSE;
}

bool WEXPORT WString::operator==( char* cstring )
{
	ifptr( cstring ) {
		return strcmp( _value, cstring ) == 0;
	}
	return FALSE;
}

WEXPORT WString::operator char*()
{
	return _value;
}


