#include "dump.h"
struct S {
    char p1[5];
    char p2[5];
};

S s1 = { "abcd", "abcd" };
S s2 = { { "abcd" }, { "abcd" } };
int main( void )
{
    DUMP( s1 );
    DUMP( s2 );
    return 0;
}
