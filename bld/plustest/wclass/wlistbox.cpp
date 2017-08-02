#include "wlistbox.hpp"
#include "wstring.hpp"

WEXPORT WListBox::WListBox( WWindow* parent, const WRect& r, char* text, WStyle wstyle )
        : WControl( parent, "LISTBOX", r, text, wstyle )
        , _changedClient( NIL )
        , _changed( NIL )
        , _dblClickClient( NIL )
        , _dblClick( NIL )
{
}

WEXPORT WListBox::~WListBox()
{
}

void WEXPORT WListBox::onChanged( WObject* client, cb changed )
{
        _changedClient = client;
        _changed = changed;
}

void WEXPORT WListBox::onDblClick( WObject* client, cb click )
{
        _dblClickClient = client;
        _dblClick = click;
}

bool WEXPORT WListBox::processCmd( WORD id, WORD code )
{
        switch( code ) {
        case LBN_SELCHANGE:
                if( isnil( _changedClient ) || isnil( _changed ) ) break;
                (_changedClient->*_changed)();
                return TRUE;
        case LBN_DBLCLK:
                if( isnil( _dblClickClient ) || isnil( _dblClick ) ) break;
                (_dblClickClient->*_dblClick)();
                return TRUE;
        }
		return WControl::processCmd( id, code );
}

WString WEXPORT WListBox::getString( int index )
{
        int len = sendMsg( LB_GETTEXTLEN, index, 0 );
        WString s( len );
        sendMsg( LB_GETTEXT, index, (DWORD)(char WFAR*)s );
        return s;
}

void WEXPORT WListBox::insertString( char* s, int index )
{
        sendMsg( LB_INSERTSTRING, index, (DWORD)(char WFAR*)s );
}

void WEXPORT WListBox::deleteString( int index )
{
        sendMsg( LB_DELETESTRING, index, 0 );
}

void WEXPORT WListBox::reset()
{
        sendMsg( LB_RESETCONTENT, 0, 0 );
}

int WEXPORT WListBox::count()
{
        return sendMsg( LB_GETCOUNT, 0, 0 );
}

int WEXPORT WListBox::selected()
{
        return sendMsg( LB_GETCURSEL, 0, 0 );
}

void WEXPORT WListBox::select( int index )
{
        sendMsg( LB_SETCURSEL, index, 0 );
}


