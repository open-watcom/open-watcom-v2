#include "fail.h"

class uint64 {
    public:
        uint64()                        { }
        uint64( unsigned long i )       { lo = i; }
        uint64 operator+( const uint64 & a ) { uint64 tmp; tmp.lo = lo + a.lo; return tmp; }
        int operator<( const uint64 &a ) { return lo < a.lo; }
        uint64 operator+=( const uint64 & a )
        {
            // The assignment never gets done.
            return( *this = *this + a );
            // The following works though:
            // *this = *this + a;
            // return( *this );
        }
        uint64 operator++()
        {
            return( *this += 1 );
        }

        unsigned        hi, lo;
};
inline unsigned truncate( const uint64 &a )
{
    return( a.lo );
}


main()
{
    uint64 i;
    unsigned ui = 0;


    for( i = 0; i < 10; ++i, ++ui ) {
        unsigned tr = truncate( i );
        if( ui != tr ) _fail;
    }
    _PASS
}
