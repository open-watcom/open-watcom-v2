#ifndef wcombox_class
#define wcombox_class

#include "wcontrol.hpp"
#include "wstring.hpp"

WCLASS WComboBox : public WControl
{
	public:
		WEXPORT WComboBox( WWindow* parent, const WRect& r, char* text, WStyle wstyle=CBS_HASSTRINGS|CBS_DROPDOWNLIST|WS_VSCROLL );
		WEXPORT ~WComboBox();
		void WEXPORT onChanged( WObject* obj, cb changed );
		void WEXPORT onDblClick( WObject* obj, cb dblClick );
		bool WEXPORT processCmd( WORD id, WORD code );

		WString WEXPORT getString( int index );
		void WEXPORT insertString( char* s, int index=-1 );
		void WEXPORT deleteString( int index );
		void WEXPORT reset();
		int WEXPORT count();
		int WEXPORT selected();
		void WEXPORT select( int index );
	private:
		WObject*	_changedClient;
		cb		_changed;
		WObject*	_dblClickClient;
		cb		_dblClick;
};

#endif
