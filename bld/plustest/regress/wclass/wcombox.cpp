#include "wcombox.hpp"

WEXPORT WComboBox::WComboBox( WWindow* parent, const WRect& r, char* text, WStyle wstyle )
        : WControl( parent, "COMBOBOX", r, text, wstyle )
        , _changedClient( NIL )
        , _changed( NIL )
        , _dblClickClient( NIL )
        , _dblClick( NIL )
{
}

WEXPORT WComboBox::~WComboBox()
{
}

void WEXPORT WComboBox::onChanged( WObject* client, cb changed )
{
        _changedClient = client;
        _changed = changed;
}

void WEXPORT WComboBox::onDblClick( WObject* client, cb click )
{
        _dblClickClient = client;
        _dblClick = click;
}

bool WEXPORT WComboBox::processCmd( WORD id, WORD code )
{
        switch( code ) {
        case CBN_SELCHANGE:
                if( isnil( _changedClient ) || isnil( _changed ) ) break;
                (_changedClient->*_changed)();
                return TRUE;
        case CBN_DBLCLK:
                if( isnil( _dblClickClient ) || isnil( _dblClick ) ) break;
                (_dblClickClient->*_dblClick)();
                return TRUE;
        }
		return WControl::processCmd( id, code );
}

WString WEXPORT WComboBox::getString( int index )
{
        int len = sendMsg( CB_GETLBTEXTLEN, index, 0 );
        WString s( len );
        sendMsg( CB_GETLBTEXT, index, (DWORD)(char WFAR*)s );
        return s;
}

void WEXPORT WComboBox::insertString( char* s, int index )
{
        sendMsg( CB_INSERTSTRING, index, (DWORD)(char WFAR*)s );
}

void WEXPORT WComboBox::deleteString( int index )
{
        sendMsg( CB_DELETESTRING, index, 0 );
}

void WEXPORT WComboBox::reset()
{
        sendMsg( CB_RESETCONTENT, 0, 0 );
}

int WEXPORT WComboBox::count()
{
        return sendMsg( CB_GETCOUNT, 0, 0 );
}

int WEXPORT WComboBox::selected()
{
        return sendMsg( CB_GETCURSEL, 0, 0 );
}

void WEXPORT WComboBox::select( int index )
{
        sendMsg( CB_SETCURSEL, index, 0 );
		processCmd( 0, CBN_SELCHANGE);     //should we have this??
}


