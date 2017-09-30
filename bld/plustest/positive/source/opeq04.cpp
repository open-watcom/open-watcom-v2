#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning 549 9

#pragma pack(4);

struct V {
};
struct S : virtual V {
};
struct P {
};
struct T : P, S {
};

void foo( S &x, S &y )
{
    x = y;
}

char dest[ sizeof(T) ];
char src[ sizeof(T) ];

int main()
{
    memset( dest, 'd', sizeof(T) );
    memset( src, 's', sizeof(T) );
    T &d = *(new (dest) T);
    T &s = *(new (src) T);
    foo( d, s );
    unsigned not_copied = 0;
    for( int i = 0; i < sizeof(T); ++i ) {
	if( dest[i] == 'd' && src[i] == 's' ) {
	    ++not_copied;
	}
    }
    not_copied %= sizeof( int );
    if( not_copied ) fail(__LINE__);
    _PASS;
}
