#include "fail.h"

//#if defined( M_I86 ) || defined( M_I386 )

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
