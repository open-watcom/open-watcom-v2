#include "dump.h"

char p1[5] = { "abcd" };
char p2[5] = { 'a', 'b', 'c', 'd', '\x0' };

int main( void )
{
    DUMP( p1 );
    DUMP( p2 );
    return 0;
}
