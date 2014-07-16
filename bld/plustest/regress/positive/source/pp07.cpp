#include "fail.h"

#define PASTE_(x,y) x##y
#define PASTE(x,y) PASTE_(x,y)
#define MANGLE_NAME(name) PASTE(PASTE(Mangle,_),name)

#define def(op,name) int MANGLE_NAME(name)( int x, int y ) { return x op y; }

def(+,add)
def(-,minus)

int main()
{
    if( Mangle_add( 1, 2 ) != 3 ) fail(__LINE__);
    if( Mangle_minus( 3, 4 ) != -1 ) fail(__LINE__);
    _PASS;
}
