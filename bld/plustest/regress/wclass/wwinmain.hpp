#ifndef wwinmain_module
#define wwinmain_module

#include "wwindow.hpp"
#include "wmdiwndw.hpp"

extern HWND	_appPrev;
extern HWND	_appInst;
extern int  _appShow;

extern UINT dispatch();
extern UINT _A_wmain( LPSTR cmd, HWND inst );

#endif
