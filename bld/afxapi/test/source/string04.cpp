#include "stdafx.h"

// This program tests loading a string resource.  It must be linked with string04.rc.
int main() {
    CString str;
    str.LoadString( 1 );
    if( str != _T("abcd") ) _fail;

    _PASS;
}
