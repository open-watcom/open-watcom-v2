// tested under Borland C++
#include "fail.h"

unsigned trace[1000];
unsigned p;
unsigned x;
unsigned should[1000] = {
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0105,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0105,
    0x0106,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0105,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0101,
    0x0101,
    0x0102,
    0x0101,
    0x0101,
    0x0102,
    0x0103,
    0x0104,
    0x0105,
    0x0106,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0201,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0303,
    0x0405,
    0x0405,
    0x0405,
    0x0405,
    -1,
};
void ctor( unsigned i ) {
    if( p == x ) {
	throw p;
    }
    if( should[p] == i ) {
	//printf( "ctor: %04x\n", i );
	trace[p++] = i;
    } else {
	printf( "sequence: %u was %04x (should be %04x)\n", p, i, should[p] );
	_fail;
    }
}
void dtor( unsigned i ) {
    if( p == 0 ) {
	_fail;
    } else {
	--p;
	//printf( "dtor: %04x\n", i );
	if( should[p] != i ) {
	    printf( "sequence: %u was %04x (should be %04x)\n", p, i, should[p] );
	    _fail;
	}
    }
}

#define MAKE_MESS( pref, m ) \
    struct pref##1 { \
	char a[13]; \
	pref##1() { \
	    ctor( m | 1 ); \
	} \
	~pref##1() { \
	    dtor( m | 1 ); \
	} \
    }; \
    struct pref##2 : pref##1 { \
	pref##1 v; \
	pref##2() { \
	    ctor( m | 2 ); \
	} \
	~pref##2() { \
	    dtor( m | 2 ); \
	} \
    }; \
    struct pref##3 : virtual pref##2 { \
	pref##2 v; \
	pref##3() { \
	    ctor( m | 3 ); \
	} \
	~pref##3() { \
	    dtor( m | 3 ); \
	} \
    }; \
    struct pref##4 : pref##3 { \
	pref##3 v; \
	pref##4() { \
	    ctor( m | 4 ); \
	} \
	~pref##4() { \
	    dtor( m | 4 ); \
	} \
    }; \
    struct pref##5 : virtual pref##4 { \
	pref##4 v; \
	pref##5() { \
	    ctor( m | 5 ); \
	} \
	~pref##5() { \
	    dtor( m | 5 ); \
	} \
    }; \
    struct pref##6 : pref##5 { \
	pref##5 v; \
	pref##6() { \
	    ctor( m | 6 ); \
	} \
	~pref##6() { \
	    dtor( m | 6 ); \
	} \
    }; \

#define MAKE_MESS1( pref, m ) \
    struct pref##1 { \
	char a[13]; \
	pref##1() { \
	    ctor( m | 1 ); \
	} \
	~pref##1() { \
	    dtor( m | 1 ); \
	} \
    }; \
    struct pref##2 : pref##1 { \
	pref##1 v; \
    }; \
    struct pref##3 : virtual pref##2 { \
	pref##2 v; \
    }; \
    struct pref##4 : pref##3 { \
	pref##3 v; \
    }; \
    struct pref##5 : virtual pref##4 { \
	pref##4 v; \
    }; \
    struct pref##6 : pref##5 { \
	pref##5 v; \
    }; \

#define MAKE_MESS3( pref, m ) \
    struct pref##1 { \
	char a[13]; \
    }; \
    struct pref##2 : pref##1 { \
	pref##1 v; \
    }; \
    struct pref##3 : virtual pref##2 { \
	pref##2 v; \
	pref##3() { \
	    ctor( m | 3 ); \
	} \
	~pref##3() { \
	    dtor( m | 3 ); \
	} \
    }; \
    struct pref##4 : pref##3 { \
	pref##3 v; \
    }; \
    struct pref##5 : virtual pref##4 { \
	pref##4 v; \
    }; \
    struct pref##6 : pref##5 { \
	pref##5 v; \
    }; \

#define MAKE_MESS5( pref, m ) \
    struct pref##1 { \
	char a[13]; \
    }; \
    struct pref##2 : pref##1 { \
	pref##1 v; \
    }; \
    struct pref##3 : virtual pref##2 { \
	pref##2 v; \
    }; \
    struct pref##4 : pref##3 { \
	pref##3 v; \
    }; \
    struct pref##5 : virtual pref##4 { \
	pref##4 v; \
	pref##5() { \
	    ctor( m | 5 ); \
	} \
	~pref##5() { \
	    dtor( m | 5 ); \
	} \
    }; \
    struct pref##6 : pref##5 { \
	pref##5 v; \
    }; \

MAKE_MESS( A, 0x0100 )
MAKE_MESS1( B, 0x0200 )
MAKE_MESS3( C, 0x0300 )
MAKE_MESS5( D, 0x0400 )

struct V1 : virtual A6 {
    A6 v;
};
struct V2 : virtual V1, B6 {
    B6 v;
};
struct V3 : virtual V2, virtual C6 {
    C6 v;
};
struct V4 : virtual V3, D6 {
    D6 v;
};

int main() {
    x = -1;
    p = 0;
    {
	V4 x;
	if( p != 210 ) _fail;
	if( should[p] != -1 ) _fail;
    }
    if( p != 0 ) _fail;
    for( x = 0; x < 210+1; ++x ) {
	//printf( "x: %u\n", x );
	p = 0;
	try {
	    V4 v;
	    if( x < 210 ) _fail;
	} catch( unsigned v ) {
	    if( v != x ) _fail;
	    if( p != 0 ) _fail;
	} catch( ... ) {
	    _fail;
	}
    }
    _PASS;
}
