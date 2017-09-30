#include <stdio.h>

#define COUNT 1000
unsigned count;

void f1()
{
    SBIG i;

    for( i = 0; i < COUNT; ++i ) {
        ++count;
    }
}

void f2()
{
    SBIG i;

    for( i = 0; i < 2*COUNT; ++i ) {
        ++count;
    }
}

void f3()
{
    SBIG i;

    for( i = 0; i < 3*COUNT; ++i ) {
        ++count;
    }
}

void f4()
{
    SBIG i;

    for( i = 0; i < 4*COUNT; ++i ) {
        ++count;
    }
}

int main()
{
    SBIG i;

    for( i = 0; i < 10000; ++i ) {
        f1();
        f2();
        f3();
        f4();
    }
    printf( "PASSED\n" );
    return 0;
}
