#include "wwinmain.hpp"

HINSTANCE   _appPrev;
HINSTANCE   _appInst;
int         _appShow;

extern "C" int _pascal WinMain( HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show )
{
#if !defined( __NT__ )
	if( GetWinFlags() & WF_PMODE ) {
#endif
		_appPrev = prev;
		_appInst = inst;
		_appShow = show;
		return _A_wmain( cmd, inst );
#if !defined( __NT__ )
	}
	return 0;
#endif
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


