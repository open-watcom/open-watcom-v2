#include "fail.h"

unsigned x = sizeof( "123" );
unsigned y = sizeof( L"12345" );

int main()
{
    if( x != 4 ) _fail;
    if( y != 6*2 ) _fail;
    _PASS;
}
