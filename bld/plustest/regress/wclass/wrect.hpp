#ifndef wrect_class
#define wrect_class

#include "wregion.hpp"

WCLASS WRect : public WRegion
{
	public:
		WEXPORT WRect( int x=0, int y=0, int w=0, int h=0 )
			: _x( x ), _y( y ), _w( w ), _h( h ) {}
		WEXPORT ~WRect() {}
		int WEXPORT x() const { return _x; }
		int WEXPORT y() const { return _y; }
		int WEXPORT w() const { return _w; }
		int WEXPORT h() const { return _h; }
		void WEXPORT x( int x ) { _x = x; }
		void WEXPORT y( int y ) { _y = y; }
		void WEXPORT w( int w ) { _w = w; }
		void WEXPORT h( int h ) { _h = h; }

		void WEXPORT fitInside( const WRect& r );

	private:
		int _x, _y, _w, _h;
};

#endif
