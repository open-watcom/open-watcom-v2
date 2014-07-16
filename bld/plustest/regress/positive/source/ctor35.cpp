#include "fail.h"

unsigned trace[1000];
unsigned p;
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
void use( unsigned i ) {
    if( should[p] == i ) {
	trace[p++] = i;
    } else {
	printf( "sequence: %u was %04x (should be %04x)\n", p, i, should[p] );
	_fail;
    }
}

#define MAKE_MESS( pref, m ) \
    struct pref##1 { \
	char a[13]; \
	pref##1() { \
	    use( m | 1 ); \
	} \
    }; \
    struct pref##2 : pref##1 { \
	pref##1 v; \
	pref##2() { \
	    use( m | 2 ); \
	} \
    }; \
    struct pref##3 : virtual pref##2 { \
	pref##2 v; \
	pref##3() { \
	    use( m | 3 ); \
	} \
    }; \
    struct pref##4 : pref##3 { \
	pref##3 v; \
	pref##4() { \
	    use( m | 4 ); \
	} \
    }; \
    struct pref##5 : virtual pref##4 { \
	pref##4 v; \
	pref##5() { \
	    use( m | 5 ); \
	} \
    }; \
    struct pref##6 : pref##5 { \
	pref##5 v; \
	pref##6() { \
	    use( m | 6 ); \
	} \
    }; \

#define MAKE_MESS1( pref, m ) \
    struct pref##1 { \
	char a[13]; \
	pref##1() { \
	    use( m | 1 ); \
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
	    use( m | 3 ); \
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
	    use( m | 5 ); \
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
    p = 0;
    { V4 x; };
    if( should[p] != -1 ) _fail;
    _PASS;
}
