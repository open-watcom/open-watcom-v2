#include "fail.h"
// Code references unimplemented copy constructor.

#include <stdio.h>

struct Q {
    Q();
    ~Q();
    __cdecl Q( const Q & );             // declared but not implemented
    __cdecl operator = ( const Q & );   // to prevent copying of objects
};
Q::Q() {};
Q::~Q() {};
int main( void ) {
    Q q;
    _PASS;
}
