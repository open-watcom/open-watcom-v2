#include "dump.h"

struct A {
    int a;
    };

void g( char ) BAD;
void g( short ) BAD;

int operator + ( A, void(*)( char ) ) { GOOD; return 0; }
int operator + ( A, short ) { BAD; return 0; }

void foo( void )
{
    A a;
    operator + ( a, g );
    operator + ( a, &g );
}
int main( void ) {
    foo();
    CHECK_GOOD( 20 );
    return errors != 0;
}
