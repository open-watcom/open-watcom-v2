#include "fail.h"

struct ManyCtors {
	int flag;
	ManyCtors() {
	    //printf( "ManyCtors(%p)\n", this );
	    flag = 0;
	}
	ManyCtors( const ManyCtors& mn ) { 
	    //printf( "ManyCtors(%p,%p(%d))\n", this, &mn, mn.flag );
	    flag = mn.flag + 1;
	}
	ManyCtors( const ManyCtors& mn, int i) { 
	    //printf( "ManyCtors(%p,%p(%d),%d)\n", this, &mn, mn.flag, i );
	    flag = i;
	}
};

ManyCtors fn( const ManyCtors& mn )
{ 
    ManyCtors temp(mn,1);
    //printf( "temp; %p(%d)\n", &temp, temp.flag );
    return temp;
}


int main()
{
    ManyCtors m1;
    ManyCtors test(fn(m1));
    //printf( "test; %p(%d)\n", &test, test.flag );
    if( test.flag < 1 || test.flag > 2 ) fail(__LINE__);
    _PASS;
}
