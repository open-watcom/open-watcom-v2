#include <stdio.h>
int main( void )
{
    int i = 0;
    int &r = i;
    printf( "%d %d\n", i, r );
    r = 1;
    int *p = &r;
    int &rr = r;
    printf( "%d %d %d %d\n", i, r, *p, rr );
    return 0;
}
