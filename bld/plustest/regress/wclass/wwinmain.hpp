#ifndef wwinmain_module
#define wwinmain_module

#include "wwindow.hpp"
#include "wmdiwndw.hpp"

extern HINSTANCE    _appPrev;
extern HINSTANCE    _appInst;
extern int          _appShow;

extern UINT dispatch();
extern UINT _A_wmain( LPSTR cmd, HINSTANCE inst );

#endif
