#include "dump.h"
char *p[3] = {
    "abc",
    "def",
    "ghi"
};

char a[4] = "abc";

char a1[3][4] = {
    "abc",
    "def",
    "ghi"
};

char a2[3][4] = {
    { "abc" },
    { "def" },
    { "ghi" }
};

char a3[3][4] = {
    'a', 'b', 'c', '\x0', 'd', 'e', 'f', '\x0', 'g', 'h', 'i', '\x0',
};

char a4[3][4] = {
    { 'a', 'b', 'c' },
    { 'd', 'e', 'f' },
    { 'g', 'h', 'i' }
};

char a5[3][4] = {
    { { 'a' }, { 'b' }, { 'c' } },
    { { 'd' }, { 'e' }, { 'f' } },
    { { 'g' }, { 'h' }, { 'i' } }
};
int main( void )
{
    puts( p[0] );
    puts( p[1] );
    puts( p[2] );
    DUMP( a );
    DUMP( a1 );
    DUMP( a2 );
    DUMP( a3 );
    DUMP( a4 );
    DUMP( a5 );
    return 0;
}
