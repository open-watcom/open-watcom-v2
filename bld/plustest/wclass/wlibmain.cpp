#include "wlibmain.hpp"

int _pascal LibMain( HANDLE /*inst*/, WORD /*dataseg*/, WORD /*heapsize*/, LPSTR /*cmd*/ )
{
	return TRUE;
}

int _far _pascal WEP( int /*code*/ )
{
	return TRUE;
}
