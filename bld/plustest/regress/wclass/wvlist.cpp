#include "wvlist.hpp"

WEXPORT WVList::WVList()
		: _set( NIL )
		, _count( 0 )
		, _free( 0 )
{
}

WEXPORT WVList::WVList( WVList& x )
		: _set( NIL )
		, _count( x._count )
		, _free( x._free )
{
	ifptr( x._set ) {
		_set = new WObject*[ _count ];
		for( int i=0; i<_count; i++ ) {
			_set[i] = x._set[i];
		}
	}
}

WEXPORT WVList::~WVList()
{
}

WObject* WEXPORT WVList::find( WObject* obj )
{
	for( int i=0; i<_free; i++ ) {
		if( _set[i]->isEqual( obj ) ) {
			return _set[ i ];
		}
	}
	return NIL;
}

WObject* WEXPORT WVList::add( WObject* obj )
{
	ifnil( _set ) {
		static _countInit = 10;
		ifptr( _set = new WObject*[ _countInit ] ) {
			_count = _countInit;
			_free = 0;
		}
	}
	ifptr( _set ) {
		if( _free < _count ) {
			_set[ _free ] = obj;
			_free += 1;
			return obj;
		} else {
			static _countIncr = 5;
			WObject** nset = new WObject*[ _count + _countIncr ];
			ifptr( nset ) {
				for( int i=0; i<_count; i++ ) {
					nset[i] = _set[i];
				}
				delete _set;
				_set = nset;
				_count += _countIncr;
				_set[ _free ] = obj;
				_free += 1;
				return obj;
			}
		}
	}
	return NIL;
}

int WEXPORT WVList::indexOfSame( WObject* obj )
{
	for( int i=0; i<_free; i++ ) {
		if( obj == _set[ i ] ) {
			return i;
		}
	}
	return -1;
}

WObject* WEXPORT WVList::removeAt( int i )
{
	if( i >= 0 && i < _free ) {
		WObject* obj = _set[ i ];
		for( ; i< _count-1; i++ ) {
			_set[ i ] = _set[ i+1 ];
		}
		_free -= 1;
		return obj;
	}
	return NIL;
}

WObject* WEXPORT WVList::removeSame( WObject* obj )
{
	return removeAt( indexOfSame( obj ) );
}

WObject* WEXPORT WVList::first()
{
	if( _free > 0 ) {
		return _set[ 0 ];
	}
	return NIL;
}

WObject* WEXPORT WVList::last()
{
	if( _free > 0 ) {
		return _set[ _free-1 ];
	}
	return NIL;
}

WObject* WEXPORT WVList::next( WObject* obj )
{
	for( int i=0; i<_free; i++ ) {
	if( obj == _set[ i ] ) {
			if( i+1 < _free ) {
				return _set[ i+1 ];
			}
		}
	}
	return NIL;
}

WObject* WEXPORT WVList::prev( WObject* obj )
{
	for( int i=1; i<_free; i++ ) {
	if( obj == _set[ i ] ) {
			return _set[ i-1 ];
		}
	}
	return NIL;
}



