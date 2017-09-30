#include "dump.h"
struct A {
	int bit2 : 2;
};
A a = { 4 };

void f( void )
{
    a.bit2 = 3;
}
int main( void )
{
    DUMP( a );
    f();
    DUMP( a );
    return 0;
}
