#include "wwindow.hpp"
#include "wvlist.hpp"
#include "wmenu.hpp"
#include "wmenuitm.hpp"
#include "wwinmain.hpp"

WObjectMap WEXPORT WWindow::_objMap;
char* WEXPORT WWindow::_appName;

static unsigned int _childId = 1;       //auto-generate child window id's

bool WWindow::processMsg( UINT msg, UINT wparm, LONG lparm )
{
	switch( msg ) {
	case WM_CLOSE:
		close();
		return TRUE;
	case WM_DESTROY:
		ifnil( GetParent( _handle ) ) {
			PostQuitMessage( 0 );
		}
		return TRUE;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		_dcHandle = BeginPaint( _handle, &ps );
		bool ret = paint();    // do the actual painting
		_dcHandle = 0;
		EndPaint( _handle, &ps );
		return ret;
	}
	case WM_COMMAND:
		if( LOWORD( lparm ) == 0 ) {
			if( HIWORD( lparm ) == 0 ) {
				//message is from a menu
				WMenuItem* itm = (WMenuItem*)WWindow::_objMap.findThis( wparm );
				ifptr( itm ) {
					itm->picked();
					return TRUE;
				}
			} else {
				//message is from an accelerator
			}
		} else {
			//message is from a control
			WWindow* ctl = (WWindow*)WWindow::_objMap.findThis( LOWORD( lparm ) );
			ifptr( ctl ) {
				if( ctl->processCmd( wparm, HIWORD( lparm ) ) ) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

bool WWindow::processCmd( WORD /*id*/, WORD /*code*/ )
{
	return FALSE;
}

extern "C" long _export _far _pascal WinProc( HWND hwin, UINT msg, UINT wparm, LONG lparm )
{
		WWindow* win = (WWindow*)WWindow::_objMap.findThis( hwin );
		ifptr( win ) {
			if( win->processMsg( msg, wparm, lparm ) ) {
				return 0;
			}
		}
		return DefWindowProc( hwin, msg, wparm, lparm );
}

bool WEXPORT WWindow::registerClass()
{
		_appName = "WWindow";
		ifptr( !_appPrev ) {
				WNDCLASS    wc;
				wc.style = CS_DBLCLKS;
#ifdef __WATCOM_CPLUSPLUS__
				wc.lpfnWndProc = (WNDPROC)WinProc;
#else
				wc.lpfnWndProc = WinProc;
#endif
				wc.cbClsExtra = 0;
                wc.cbWndExtra = 0;
				wc.hInstance = _appInst;
				wc.hIcon = LoadIcon( _appInst, IDI_APPLICATION );
                wc.hCursor = LoadCursor( NIL, IDC_ARROW );
				wc.hbrBackground = GetStockObject( WHITE_BRUSH );
				wc.lpszMenuName = NIL;
				wc.lpszClassName = _appName;
                return RegisterClass( &wc );
        }
		return TRUE;
}

void WWindow::makeWindow( char* className, char* text, WStyle wstyle )
{
		HWND hparent = NIL;
		ifptr( _parent ) hparent = _parent->_handle;
		_objMap.currThis( this );
		_handle= CreateWindow( className, text, wstyle
						, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
						, hparent, NIL, _appInst, NIL );
		ifptr( _handle ) {
				_objMap.setThis( this, _handle );
//				ShowWindow( _handle, _appShow );
		}
}

WEXPORT WWindow::WWindow()
		: _dcHandle( 0 )
		, _parent( NIL )
		, _menu( NIL )
{
}

WEXPORT WWindow::WWindow( char* text )
		: _dcHandle( 0 )
		, _parent( NIL )
		, _menu( NIL )
{
		makeWindow( _appName, text, WS_OVERLAPPEDWINDOW );
}

WEXPORT WWindow::WWindow( WWindow* parent, char* text, WStyle style )
		: _dcHandle( 0 )
		, _parent( parent )
		, _menu( NIL )
{
		makeWindow( _appName, text, style );
		ifptr( _parent ) {
				_parent->addChild( this );
		}
}

WEXPORT WWindow::WWindow( WWindow* parent, char* className, const WRect& r, char* text, WStyle wstyle )
		: _dcHandle( 0 )
		, _parent( parent )
		, _menu( NIL )
{
		HWND hparent = NIL;
		ifptr( _parent ) hparent = _parent->_handle;
		_objMap.currThis( this );
		_handle = CreateWindow( className, text, wstyle
						, r.x(), r.y(), r.w(), r.h()
						, hparent, _childId++, _appInst, NIL );
		if( _handle ) {
				_objMap.setThis( this, _handle );
		}
		ifptr( _parent ) {
				_parent->addChild( this );
		}
}

WEXPORT WWindow::~WWindow()
{
		clearMenu();
		_objMap.clearThis( this );
        while( _children.count() > 0 ) {
                delete _children.last(); //child removes self from set
		}
		ifptr( _parent ) {
				_parent->removeChild( this );
        }
		ifptr( _handle ) {
				DestroyWindow( _handle );
        }
}

void WEXPORT WWindow::addChild( WObject* child )
{
        _children.add( child );
}

void WEXPORT WWindow::removeChild( WObject* child )
{
        _children.removeSame( child );
}

DWORD WEXPORT WWindow::sendMsg( WORD msg, WORD wparm, DWORD lparm )
{
		return SendMessage( _handle, msg, wparm, lparm );
}

void WEXPORT WWindow::close()
{
		if( confirmClose() ) {
				DestroyWindow( _handle );
				_handle = NIL;
		}
}

WORD WEXPORT WWindow::getText( char* textBuf, WORD textLen )
{
		return sendMsg( WM_GETTEXT, textLen, (DWORD)(char WFAR*)textBuf );
}

WORD WEXPORT WWindow::setText( char* text )
{
		return sendMsg( WM_SETTEXT, 0, (DWORD)(char WFAR*)text );
}

void WEXPORT WWindow::setMenu( WMenu* menu )
{
        clearMenu();
		SetMenu( _handle, menu->handle() );
        _menu = menu;
        _menu->setParent( this );
}

void WEXPORT WWindow::clearMenu()
{
		SetMenu( _handle, NIL );
        ifptr( _menu ) {
				_menu->setParent( NIL );
                _menu = NIL;
        }
}

void WEXPORT WWindow::move( const WRect& r )
{
		ifptr( _handle ) {
				MoveWindow( _handle, r.x(), r.y(), r.w(), r.h(), 0 );
		}
}

WRect WEXPORT WWindow::getRectangle()
{
        WRect   r;
        RECT    wr;
		GetWindowRect( _handle, &wr );
        r.x( wr.left );
		r.y( wr.top );
        r.w( wr.right - wr.left );
		r.h( wr.bottom - wr.top );
        return r;
}

void WEXPORT WWindow::update()
{
	ifptr( _handle ) {
		UpdateWindow( _handle );
	}
}

void WEXPORT WWindow::show( int style )
{
		ifptr( _handle ) {
			if( isnil( _parent ) && isptr( _appShow ) ) {
				style = _appShow;
				_appShow = NIL;
			}
			ShowWindow( _handle, style );
//			update();
		}
}










