#include "dump.h"
int main( void )
{
    static char p[5] = { 'a', 'b', 'c', 'd', '\x0' };
    DUMP( p );
    return 0;
}
