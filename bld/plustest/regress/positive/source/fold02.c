#include "fail.h"
#include <limits.h>

#define INT_BITS	(int(CHAR_BIT * sizeof(int)))

struct uchk {
    unsigned v;
    unsigned c;
};

uchk au[] = {
    { UINT_MAX << ( INT_BITS ),		0		},
    #if 1
    { UINT_MAX << ( INT_BITS - 1 ),	INT_MIN		},
    { UINT_MAX << ( INT_BITS + 1 ), 	0		},
    { UINT_MAX << ( 2 ), 		UINT_MAX * 4	},
    { UINT_MAX << ( 0 ), 		UINT_MAX	},
    { UINT_MAX >> ( INT_BITS ), 	0		},
    { UINT_MAX >> ( INT_BITS - 1 ),	1		},
    { UINT_MAX >> ( INT_BITS + 1 ),	0		},
    { UINT_MAX >> ( 2 ),		UINT_MAX / 4	},
    { UINT_MAX >> ( 0 ),		UINT_MAX	},
    #endif
};
#define NUM_AU	(sizeof(au)/sizeof(uchk))

struct ichk {
    int v;
    int c;
};

ichk ai[] = {
    #if 1
    { INT_MAX << ( INT_BITS ),		0		},
    { INT_MAX << ( INT_BITS - 1 ),	INT_MIN		},
    { INT_MAX << ( INT_BITS + 1 ), 	0		},
    { INT_MAX << ( 2 ), 		INT_MAX * 4	},
    { INT_MAX << ( 0 ), 		INT_MAX		},
    { INT_MAX >> ( INT_BITS ), 		0		},
    { INT_MAX >> ( INT_BITS - 1 ),	0		},
    { INT_MAX >> ( INT_BITS + 1 ),	0		},
    { INT_MAX >> ( 2 ),			INT_MAX / 4	},
    { INT_MAX >> ( 0 ),			INT_MAX		},
    #endif
    { INT_MIN >> ( INT_BITS ), 		-1		},//
    { INT_MIN >> ( INT_BITS - 2 ),	~1		},//
    { INT_MIN >> ( INT_BITS + 1 ),	-1		},//
};
#define NUM_AI	(sizeof(ai)/sizeof(ichk))

int main()
{
    uchk *pu;
    ichk *pi;

    for( pu = au; pu != &au[NUM_AU]; ++pu ) {
	if( pu->v != pu->c ) fail(__LINE__);
    }
    for( pi = ai; pi != &ai[NUM_AI]; ++pi ) {
	if( pi->v != pi->c ) fail(__LINE__);
	if( pi->v != pi->c ) printf("%d\n", pi-ai);
    }
    _PASS;
}
