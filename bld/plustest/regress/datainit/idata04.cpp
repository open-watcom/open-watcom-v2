#include "dump.h"
class A { public: int a; };

class B { public: A a; int b; };

static B b = {
    { { 1 } }, { 2 }
};

int main( void )
{
    DUMP( b );
    return 0;
}
