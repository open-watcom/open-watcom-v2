#include "dump.h"

struct A {
    int a;
    void operator + ( void(*)( short ) ) GOOD;
    void operator + ( short ) BAD;
    };

void g( char ) BAD;
void g( short ) GOOD;

void foo( void )
{
    A a;

    a.operator + ( g );
    a.operator + ( &g );
    a + g;
    a + &g;
}
int main( void ) {
    foo();
    CHECK_GOOD( 20 );
    return errors != 0;
}
