#include <stdio.h>
struct D { int di; char dc; };
int main( void )
{
    int a = 1;
    int b(2);
    int c[] = { 3, { 4 }, 5, 6, 7 };
    D d = { 1, 'a' };

    printf( "a:%d b:%d "
            "c0:%d c1:%d c2:%d c3:%d c4:%d "
	    "d.di:%d d.dc:%c\n",
    	a, b,
	c[0], c[1], c[2], c[3], c[4],
	d.di, d.dc );
    return 0;
}
