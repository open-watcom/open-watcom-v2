#include "dump.h"

// from WClass for Optima++
// be sure to prefer the builting operator +(char *, int)


enum MMIdx { NULLIDX = 0 };

#if __WATCOM_REVISION__ >= 8
struct WString {
    WString( int i );
    friend WString operator+( const char * a, const WString & b ) { BAD; return WString(1); };
};
#endif

int main()
{
    const char *s;
    MMIdx	idx = NULLIDX;

    s = s + idx;
    CHECK_GOOD( 0 );
    return errors != 0;
}
