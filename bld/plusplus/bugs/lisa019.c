// from David Brandow, Optima++ bug
// now overload\udcnv14.cpp
// Status: bug fixed

#include "dump.h"
struct ResID {
    int r;
    ResID( int i ) { r = i; GOOD };
};

struct Bitmap {
    Bitmap( const ResID &  resID ) BAD
    Bitmap( const Bitmap &  bitmap ) BAD
};

void foo( ResID resID ) { GOOD; };  // 10-> ResID via ResID( int )
void foo( Bitmap  bitmap );  // 10 -> Bitmap via Bitmap( ResID( int ) )

int main()
{
    foo( 10 );
    CHECK_GOOD(12+26);
    return errors != 0;
}

