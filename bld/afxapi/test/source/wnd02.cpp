#include "stdafx.h"

int main()
{
    CWnd wnd;
    if( !wnd.Create( _T("BUTTON"), NULL, 0L, CRect( 0, 0, 0, 0 ), NULL, 0 ) ) _fail;

    if( !wnd.ModifyStyle( 0L, WS_CLIPCHILDREN | WS_CLIPSIBLINGS ) ) _fail;
    if( !(wnd.GetStyle() & (WS_CLIPCHILDREN | WS_CLIPSIBLINGS)) ) _fail;
    if( !wnd.ModifyStyle( WS_CLIPCHILDREN, WS_BORDER | WS_TABSTOP ) ) _fail;
    if( !(wnd.GetStyle() & (WS_CLIPSIBLINGS | WS_BORDER)) ) _fail;
    if( wnd.GetStyle() & WS_CLIPCHILDREN ) _fail;

    if( !wnd.ModifyStyleEx( 0L, WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST ) ) _fail;
    if( !(wnd.GetExStyle() & (WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST)) ) _fail;
    if( !wnd.ModifyStyleEx( WS_EX_NOPARENTNOTIFY, WS_EX_ACCEPTFILES ) ) _fail;
    if( !(wnd.GetExStyle() & (WS_EX_TOPMOST | WS_EX_ACCEPTFILES)) ) _fail;
    if( wnd.GetExStyle() & WS_EX_NOPARENTNOTIFY ) _fail;

    _PASS;
}
