#include "wwinmain.hpp"

HWND	_appPrev;
HWND	_appInst;
int     _appShow;

extern "C" int _pascal WinMain( HANDLE inst, HANDLE prev, LPSTR cmd, int show )
{
	if( GetWinFlags() & WF_PMODE ) {
		_appPrev = prev;
		_appInst = inst;
		_appShow = show;
		return _A_wmain( cmd, inst );
	}
	return 0;
}

UINT dispatch()
{
	MSG     msg;
	while( GetMessage( (LPMSG)&msg, NIL, 0, 0 ) ) {
		TranslateMessage( (LPMSG)&msg );
		DispatchMessage( (LPMSG)&msg );
	}
	return( msg.wParam );
}


