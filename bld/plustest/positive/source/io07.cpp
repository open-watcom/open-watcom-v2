#include "fail.h"
#include <iostream.h>
#include <strstrea.h>

static ostrstream sout;

struct C {
    ostrstream &r;
    C( ostrstream &r ) : r(r) {
    }
};

template <class T>
    C& operator <<( C& d, const T& x ) {
	d.r << x;
	return d;
    }

int main()
{
    C c(sout);
#if __WATCOM_REVISION__ >= 8
    // really checks if "reference to pointer to function" works properly
    c << "@1234@" << ends;
    char *p = sout.str();
    if( strcmp( p, "@1234@" ) != 0 ) fail(__LINE__);
#endif
    _PASS;
}
