#include "dump.h"
struct A {
	int bit1 : 1;	// 1 00
	int bit2 : 5;	// 5 01
	int bit3 : 4;	// 4 06

	int fill;

	int bit4 : 7;	// 7 00
	int bit5 : 3;	// 3 07
};

struct A a = { 1, 17, 9, 0xadde, 65, 5 };
int main( void )
{
    DUMP( a );
    return 0;
}
