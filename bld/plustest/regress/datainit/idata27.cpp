#include "dump.h"
class B {
    public:
	int b;
};

class D : public B {
    public:
    	int d;
};

D d;
B &b = d;	// ok
int main( void )
{
    DUMP( d );
    DUMP( b );
    return 0;
}
