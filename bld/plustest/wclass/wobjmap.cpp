#include "wobjmap.hpp"

class WObjectMapItem : public WObject
{
	public:
		WEXPORT WObjectMapItem( WObject* obj, HANDLE hand ) : _obj(obj), _hand(hand) {}
		WEXPORT ~WObjectMapItem() {}
		virtual bool WEXPORT isEqual( WObject* obj );
		WObject* WEXPORT obj() { return _obj; }

	private:
		WObject*		_obj;
		HANDLE			_hand;
};


bool WEXPORT WObjectMapItem::isEqual( WObject* obj )
{
	ifptr( ((WObjectMapItem*)obj)->_obj ) {
		ifptr( ((WObjectMapItem*)obj)->_hand ) {
			return ((WObjectMapItem*)obj)->_obj == _obj && ((WObjectMapItem*)obj)->_hand == _hand;
		}
		return ((WObjectMapItem*)obj)->_obj == _obj;
	} else ifptr( ((WObjectMapItem*)obj)->_hand ) {
		return ((WObjectMapItem*)obj)->_hand == _hand;
	}
	return FALSE;
}

void WEXPORT WObjectMap::setThis( WObject* obj, HANDLE hand )
{
	add( new WObjectMapItem( obj, hand ) );
	_currThis = NIL;
}

void WEXPORT WObjectMap::clearThis( WObject* obj )
{
	WObjectMapItem	x( obj, NIL );
	remove( &x );
}

WObject* WEXPORT WObjectMap::findThis( HANDLE hand )
{
	ifptr( hand ) {
		WObjectMapItem	x( NIL, hand );
		WObjectMapItem* m = (WObjectMapItem*)find( &x );
		ifnil( m ) {
			return _currThis;
		}
		return m->obj();
	}
	return NIL;
}

