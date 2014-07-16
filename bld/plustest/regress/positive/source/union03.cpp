#include "fail.h"

inline int ii( int i ) {
    union {
	int x;
	unsigned char y;
    };
    x = 0;
    y = i;
    return x;
}
inline int jj( int i ) {
    union {
	int x;
	unsigned char y;
    };
    x = i;
    ++x;
    return y-1;
}

int i1( int i ) {
    return ii( i + 1 ) + jj( i - 2 );
}
int i2( int i ) {
    return ii( i + 2 ) + jj( i - 3 );
}

int main() {
    if( i2( i1( 0x0101 ) ) != 0x0101 ) _fail;
    _PASS;
}
