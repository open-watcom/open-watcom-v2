#include "wmdiwndw.hpp"
#include "wwinmain.hpp"
#include "wmdichld.hpp"
#include "wmenu.hpp"

char* WEXPORT WMdiChild::_childName;
#define _mdiParent ((WMdiWindow*)_parent)

extern "C" long _export _far _pascal ChildProc( HWND hwin, UINT msg, UINT wparm, LONG lparm )
{
		WMdiWindow* win = (WMdiWindow*)WWindow::_objMap.findThis( hwin );
		ifptr( win ) {
			switch( msg ) {
			default:
				if( win->processMsg( msg, wparm, lparm ) ) {
					return 0;
				}
			}
		}
		return DefMDIChildProc( hwin, msg, wparm, lparm );
}

bool WEXPORT WMdiChild::registerClass()
{
		_childName = "WChild";
		ifptr( !_appPrev ) {
				WNDCLASS    wc;
				wc.style = 0;
#ifdef __WATCOM_CPLUSPLUS__
				wc.lpfnWndProc = (WNDPROC)ChildProc;
#else
				wc.lpfnWndProc = ChildProc;
#endif
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hInstance = _appInst;
				wc.hIcon = LoadIcon( _appInst, IDI_APPLICATION );
				wc.hCursor = LoadCursor( NIL, IDC_ARROW );
				wc.hbrBackground = GetStockObject( WHITE_BRUSH );
				wc.lpszMenuName = NIL;
				wc.lpszClassName = _childName;
				return RegisterClass( &wc );
		}
		return TRUE;
}

WEXPORT WMdiChild::WMdiChild( WMdiWindow* parent, char* text )
{
//	_mdiParent = parent;
	_parent = parent;
	MDICREATESTRUCT cc;
	cc.szClass = _childName;
	cc.szTitle = text;
	cc.hOwner = _appInst;
	cc.x = CW_USEDEFAULT;
	cc.cx = CW_USEDEFAULT;
	cc.y = CW_USEDEFAULT;
	cc.cy = CW_USEDEFAULT;
	cc.style = 0;
	cc.lParam = 0L;
	_objMap.currThis( this );
	_handle = _mdiParent->sendClientMsg( WM_MDICREATE, 0, (LONG)(LPMDICREATESTRUCT)&cc );
	ifptr( _handle ) {
		_objMap.setThis( this, _handle );
	}
}

WEXPORT WMdiChild::~WMdiChild()
{
}

void WEXPORT WMdiChild::makeActive()
{
	PostMessage( _mdiParent->clientHandle(), WM_MDIACTIVATE, _handle, 0L );
}

bool WMdiChild::processMsg( UINT msg, UINT wparm, LONG lparm )
{
	switch( msg ) {
	case WM_MDIACTIVATE:
		if( this == (WMdiChild*)WWindow::_objMap.findThis( HIWORD( lparm ) ) ) {
			activate( FALSE );
		}
		if( this == (WMdiChild*)WWindow::_objMap.findThis( LOWORD( lparm ) ) ) {
			activate( TRUE );
		}
		return TRUE;
	}
	return WWindow::processMsg( msg, wparm, lparm );
}

void WEXPORT WMdiChild::close()
{
	show( SW_SHOWMINIMIZED );
}

void WMdiChild::insertPopup( WPopupMenu* pop, int index )
{
	_mdiParent->insertPopup( pop, index );
}

void WMdiChild::removePopup( WPopupMenu* pop )
{
	_mdiParent->removePopup( pop );
}


