#ifndef wport_class
#define wport_class

#include "wwindow.hpp"
#include "wrect.hpp"

WCLASS WFont;
WCLASS WBrush;
WCLASS WPen;
WCLASS WPalette;
WCLASS WPort : public WObject
{
	public:
		WEXPORT WPort( WWindow* host );
		WEXPORT ~WPort();

		void WEXPORT setFont( WFont* font ) { _font = font; }
		void WEXPORT setBrush( WBrush* brush ) { _brush = brush; }
		void WEXPORT setPen( WPen* pen ) { _pen = pen; }
		void WEXPORT setPalette( WPalette* palette ) { _palette = palette; }

		void WEXPORT moveTo() {}
		void WEXPORT lineTo() {}
		void WEXPORT rectangle() {}
		void WEXPORT ellipse() {}
		void WEXPORT arc() {}
		void WEXPORT pie() {}
		void WEXPORT textOut( const WRect& r, char* text );
		void WEXPORT drawText() {}

	private:
		HDC		_dc;
		WWindow*	_host;
		WFont*		_font;
		WBrush*		_brush;
		WPen*		_pen;
		WPalette*	_palette;
};

#endif




