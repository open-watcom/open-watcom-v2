#include "dump.h"

class complex {
    public:
	float f;
	complex();
	complex( double );
	complex( double, double );
};

complex	v[6] = { complex(1),
                 complex(2,3),
		 complex(),
		 complex(4) };   // pad with complex()

complex::complex()
{
    f = 0.0;
    printf( "CTOR1:%x\n", this - v );
}
complex::complex( double d )
{
    f = (float)d;
    printf( "CTOR2:%x\n", this - v);
}
complex::complex( double d, double e )
{
    f = (float)(d*e);
    printf( "CTOR3:%x\n", this - v);
}

int main( void )
{
    DUMP( v );
    return 0;
}
