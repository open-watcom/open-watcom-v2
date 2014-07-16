#include "fail.h"

struct Test {
    Test(unsigned val) : value(val) {
	printf( "Test(%p,%u)\n", this, value );
	if( value > 2 ) _fail;
    }
    Test(const Test& other) {
	value = other.value;
	if( value > 2 ) _fail;
	printf( "Test(%p,%p,%u)\n", this, &other, value );
    }
    ~Test() {
	if( value > 2 ) _fail;
	printf( "~Test(%p,%u)\n", this, value );
    }
    unsigned value;
};

void zap() {
    char buff[100];
    memset( buff, -3, sizeof(buff) );
}
void foo( int b ) {
    Test a(1);
    Test const & test = b ? a : Test(2);
}
 
int main() {
    zap();
    foo( 0 );
    zap();
    foo( 1 );
    _PASS;
}
