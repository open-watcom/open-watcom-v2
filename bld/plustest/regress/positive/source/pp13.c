#include "fail.h"

#define paste( x, y )  x##y
double d = paste( 0189,45.125 );

int main()
{
    if( d != 18945.125 ) fail(__LINE__);
    _PASS;
}
