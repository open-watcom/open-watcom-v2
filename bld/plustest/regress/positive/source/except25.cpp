#include "fail.h"

//#if defined( _M_IX86 )

#include <string.hpp>

// stack corrupted message

int main()
{
    String input("test.cpp");

    try {
        int a = 3;
    } catch (...) {
    }
    {
        long oldflags = cout.setf(ios::dec);
    }
    _PASS;
}

//#else
//
//ALWAYS_PASS
//
//#endif
