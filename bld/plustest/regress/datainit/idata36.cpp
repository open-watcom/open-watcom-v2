#include "dump.h"
struct S {
    char *p;
};

S s = { "abcd" };
int main( void )
{
    puts( s.p );
    return 0;
}
