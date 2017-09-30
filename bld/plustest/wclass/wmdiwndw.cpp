#include "wmdiwndw.hpp"
#include "wwinmain.hpp"
#include "wmdichld.hpp"

#include "wmenu.hpp"
#include "wmenuitm.hpp"
#include "wpopmenu.hpp"

extern "C" long _export _far _pascal FrameProc( HWND hwin, UINT msg, UINT wparm, LONG lparm )
{
		WMdiWindow* win = (WMdiWindow*)WWindow::_objMap.findThis( (HANDLE)hwin );
		ifptr( win ) {
			switch( msg ) {
			case WM_CREATE:
				win->createClient( hwin );
				return 0;
			default:
				if( win->processMsg( msg, wparm, lparm ) ) {
					return 0;
				}
			}
			return DefFrameProc( hwin, win->clientHandle(), msg, wparm, lparm );
		}
		return DefFrameProc( hwin, NIL, msg, wparm, lparm );
}

bool WEXPORT WMdiWindow::registerClass()
{
		_appName = "WFrame";
		ifptr( !_appPrev ) {
				WNDCLASS    wc;
				wc.style = 0;
#ifdef __WATCOM_CPLUSPLUS__
				wc.lpfnWndProc = (WNDPROC)FrameProc;
#else
				wc.lpfnWndProc = FrameProc;
#endif
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hInstance = _appInst;
				wc.hIcon = LoadIcon( _appInst, IDI_APPLICATION );
				wc.hCursor = LoadCursor( NIL, IDC_ARROW );
				wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE+1;
				wc.lpszMenuName = NIL;
				wc.lpszClassName = _appName;
				if( RegisterClass( &wc ) ) {
					return WMdiChild::registerClass();
				}
				return FALSE;
		}
		return TRUE;
}

WEXPORT WMdiWindow::WMdiWindow( char* text )
	: WWindow( text )
//	, _clientHandle( NIL )		//initialized by WM_CREATE code
	, _winPopup( NIL )
{
}

WEXPORT WMdiWindow::~WMdiWindow()
{
//	ifptr( _winPopup ) {
		delete _winPopup;
//	}
}

void WEXPORT WMdiWindow::createClient( HWND hwin )
{
	CLIENTCREATESTRUCT cs;
	cs.hWindowMenu = NIL;
	cs.idFirstChild = 100;
	_clientHandle = CreateWindow( "MDICLIENT", NIL, WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL|WS_HSCROLL
			, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
			, hwin, (HMENU)0xCAC, _appInst, (LPSTR)&cs );
	ifptr( _clientHandle ) {
		ShowWindow( _clientHandle, SW_SHOW );
	}
}

DWORD WEXPORT WMdiWindow::sendClientMsg( WORD msg, WORD wparm, DWORD lparm )
{
	return SendMessage( _clientHandle, msg, wparm, lparm );
}

void WEXPORT WMdiWindow::setMenu( WMenu* menu )
{
	WWindow::setMenu( menu );
	sendClientMsg( WM_MDISETMENU, 0, MAKELONG( _menu->handle(), _winPopup->handle() ) );
}

void WEXPORT WMdiWindow::clearMenu()
{
	sendClientMsg( WM_MDISETMENU, 0, MAKELONG( 0, 0 ) );
	WWindow::clearMenu();
}

void WMdiWindow::insertPopup( WPopupMenu* pop, int index )
{
	if( isptr( pop ) && isptr( _menu ) ) {
		_menu->insertItem( pop, index );
		DrawMenuBar( _handle );
	}
}

void WMdiWindow::removePopup( WPopupMenu* pop )
{
	if( isptr( pop ) && isptr( _menu ) ) {
		_menu->removeItem( pop );
		DrawMenuBar( _handle );
	}
}

WPopupMenu* WEXPORT WMdiWindow::getMdiPopup()
{
	ifnil( _winPopup ) {
		_winPopup = new WPopupMenu( "&Window" );
		_winPopup->insertItem( new WMenuItem( "&Cascade", this, (cb)&WMdiWindow::cascadeChildren ), 0 );
		_winPopup->insertItem( new WMenuItem( "&Tile", this, (cb)&WMdiWindow::tileChildren ), 1 );
		_winPopup->insertItem( new WMenuItem( "&Arrange Icons", this, (cb)&WMdiWindow::arrangeIcons ), 2 );
	}
	return _winPopup;
}

void WEXPORT WMdiWindow::cascadeChildren()
{
	sendClientMsg( WM_MDICASCADE, 0, 0L );
}

void WEXPORT WMdiWindow::tileChildren()
{
	sendClientMsg( WM_MDITILE, 0, 0L );
}

void WEXPORT WMdiWindow::arrangeIcons()
{
	sendClientMsg( WM_MDIICONARRANGE, 0, 0L );
}
