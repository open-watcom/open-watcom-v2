#include <stdio.h>
struct A {
    int a1;
    static int a2;
    int a3;
    enum a4 { a, b, c };
    int a5;
    typedef float a6;
    int a7;
};
int main( void )
{
    A f = { 1, 3, 5, 7 };
    printf( "%d %d %d %d\n", f.a1, f.a3, f.a5, f.a7 );
    return 0;
}
