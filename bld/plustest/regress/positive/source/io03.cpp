#include "fail.h"
#include <iostream.h>
#include <fstream.h>

int main() {
    auto char fname[ L_tmpnam ];
    auto char buff[ 80 ];

    tmpnam( fname );
    ifstream ifstr( fname );
    if( ifstr.gcount() != 0 ) fail(__LINE__);
    ifstr.read( buff, sizeof( buff ) );
    if( ifstr.gcount() != 0 ) fail(__LINE__);
    _PASS;
}
