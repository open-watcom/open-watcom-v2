#include "dump.h"
class A {
    public:
    	int a;
};

class B: public A {
    public:
    	int b;
	B() { a = 1; b = 2; }
};

B b1;
B b2 = b1;
A a = b2;
int main( void )
{
    DUMP( b1 );
    DUMP( b2 );
    DUMP( a );
    return 0;
}
