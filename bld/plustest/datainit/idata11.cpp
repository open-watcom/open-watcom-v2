#include <stdio.h>
struct A {
	int arr1[4];
	int bit1 : 1;	// 1 00
	int bit2 : 5;	// 5 01
	int arr2[4];
};

int f( void ) { return 14; };

int main( void )
{
    static A a = { { 0 }, 1, f(), { 0 } };
    printf( "%d...%d %d %d %d...%d\n",
    		a.arr1[0], a.arr1[3],
		a.bit1,
		a.bit2,
		a.arr2[0], a.arr2[3] );
    return 0;
}
