#include <stdio.h>
int a = 0;

struct X {
    static int a;
    static int b;
};

int X::a = 1;
int X::b = a;	// X::b = X::a

int main( void )
{
    printf( "%d\n", ::a );
    printf( "%d\n", X::a );
    printf( "%d\n", X::b );
    return 0;
}
