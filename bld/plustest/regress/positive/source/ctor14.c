#include "fail.h"
#include <string.h>

int count;

struct CD {
    unsigned signature;
    CD(unsigned);
    ~CD();
};

CD::CD( unsigned line ) : signature(line)
{
    ++count;
}

CD::~CD()
{
    --count;
    if( count < 0 ) fail(__LINE__);
}

int c[35];

int foo( int *p )
{
    return
	p[0] ? ( CD(__LINE__), 0 ) : (
	p[1] ? ( CD(__LINE__), 1 ) : (
	p[2] ? ( CD(__LINE__), 2 ) : (
	p[3] ? ( CD(__LINE__), 3 ) : (
	p[4] ? ( CD(__LINE__), 4 ) : (
	p[5] ? ( CD(__LINE__), 5 ) : (
	p[6] ? ( CD(__LINE__), 6 ) : (
	p[7] ? ( CD(__LINE__), 7 ) : (
	p[8] ? ( CD(__LINE__), 8 ) : (
	p[9] ? ( CD(__LINE__), 9 ) : (
	p[10] ? ( CD(__LINE__), 10 ) : (
	p[11] ? ( CD(__LINE__), 11 ) : (
	p[12] ? ( CD(__LINE__), 12 ) : (
	p[13] ? ( CD(__LINE__), 13 ) : (
	p[14] ? ( CD(__LINE__), 14 ) : (
	p[15] ? ( CD(__LINE__), 15 ) : (
	p[16] ? ( CD(__LINE__), 16 ) : (
	p[17] ? ( CD(__LINE__), 17 ) : (
	p[18] ? ( CD(__LINE__), 18 ) : (
	p[19] ? ( CD(__LINE__), 19 ) : (
	p[20] ? ( CD(__LINE__), 20 ) : (
	p[21] ? ( CD(__LINE__), 21 ) : (
	p[22] ? ( CD(__LINE__), 22 ) : (
	p[23] ? ( CD(__LINE__), 23 ) : (
	p[24] ? ( CD(__LINE__), 24 ) : (
	p[25] ? ( CD(__LINE__), 25 ) : (
	p[26] ? ( CD(__LINE__), 26 ) : (
	p[27] ? ( CD(__LINE__), 27 ) : (
	p[28] ? ( CD(__LINE__), 28 ) : (
	p[29] ? ( CD(__LINE__), 29 ) : (
	p[30] ? ( CD(__LINE__), 30 ) : (
	p[31] ? ( CD(__LINE__), 31 ) : (
	p[32] ? ( CD(__LINE__), 32 ) : (
	p[33] ? ( CD(__LINE__), 33 ) : (
	p[34] ? ( CD(__LINE__), 34 ) : (
	35
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	;
}

inline int ifoo( int *p )
{
    return
	p[0] ? ( CD(__LINE__), 0 ) : (
	p[1] ? ( CD(__LINE__), 1 ) : (
	p[2] ? ( CD(__LINE__), 2 ) : (
	p[3] ? ( CD(__LINE__), 3 ) : (
	p[4] ? ( CD(__LINE__), 4 ) : (
	p[5] ? ( CD(__LINE__), 5 ) : (
	p[6] ? ( CD(__LINE__), 6 ) : (
	p[7] ? ( CD(__LINE__), 7 ) : (
	p[8] ? ( CD(__LINE__), 8 ) : (
	p[9] ? ( CD(__LINE__), 9 ) : (
	p[10] ? ( CD(__LINE__), 10 ) : (
	p[11] ? ( CD(__LINE__), 11 ) : (
	p[12] ? ( CD(__LINE__), 12 ) : (
	p[13] ? ( CD(__LINE__), 13 ) : (
	p[14] ? ( CD(__LINE__), 14 ) : (
	p[15] ? ( CD(__LINE__), 15 ) : (
	p[16] ? ( CD(__LINE__), 16 ) : (
	p[17] ? ( CD(__LINE__), 17 ) : (
	p[18] ? ( CD(__LINE__), 18 ) : (
	p[19] ? ( CD(__LINE__), 19 ) : (
	p[20] ? ( CD(__LINE__), 20 ) : (
	p[21] ? ( CD(__LINE__), 21 ) : (
	p[22] ? ( CD(__LINE__), 22 ) : (
	p[23] ? ( CD(__LINE__), 23 ) : (
	p[24] ? ( CD(__LINE__), 24 ) : (
	p[25] ? ( CD(__LINE__), 25 ) : (
	p[26] ? ( CD(__LINE__), 26 ) : (
	p[27] ? ( CD(__LINE__), 27 ) : (
	p[28] ? ( CD(__LINE__), 28 ) : (
	p[29] ? ( CD(__LINE__), 29 ) : (
	p[30] ? ( CD(__LINE__), 30 ) : (
	p[31] ? ( CD(__LINE__), 31 ) : (
	p[32] ? ( CD(__LINE__), 32 ) : (
	p[33] ? ( CD(__LINE__), 33 ) : (
	p[34] ? ( CD(__LINE__), 34 ) : (
	35
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	)
	;
}

int main()
{
    int i;

    memset( c, 0, sizeof( c ) );
    if( foo( c ) != 35 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    for( i = 34; i >= 0; --i ) {
	c[i] = 1;
	if( foo(c) != i ) fail(__LINE__);
	if( count != 0 ) fail(__LINE__);
	count = 0;
    }
    memset( c, 0, sizeof( c ) );
    if( ifoo( c ) != 35 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    for( i = 34; i >= 0; --i ) {
	c[i] = 1;
	if( ifoo(c) != i ) fail(__LINE__);
	if( count != 0 ) fail(__LINE__);
	count = 0;
    }
    _PASS;
}
