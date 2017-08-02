#include "dump.h"
struct S {
    char p[5];
};

S s = { 'a', 'b', 'c', 'd', '\x0' };

int main( void )
{
    DUMP( s );
    return 0;
}
