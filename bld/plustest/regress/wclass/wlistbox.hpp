#ifndef wlistbox_class
#define wlistbox_class

#include "wcontrol.hpp"
#include "wstring.hpp"

WCLASS WListBox : public WControl
{
	public:
		WEXPORT WListBox( WWindow* parent, const WRect& r, char* text, WStyle wstyle=LBS_HASSTRINGS|LBS_NOTIFY|WS_CAPTION|WS_VSCROLL );
		WEXPORT ~WListBox();
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
