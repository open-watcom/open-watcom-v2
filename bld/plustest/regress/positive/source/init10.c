#include "fail.h"

int ctors;

struct CD {
    int v;
    CD( int v ) : v(v) { ++ctors; }
    ~CD(){
	--ctors;
	if( ctors < 0 ) fail(__LINE__);
    }
};

const CD & rCD = CD(1);

int main()
{
    if( rCD.v != 1 ) fail(__LINE__);
    _PASS;
}
