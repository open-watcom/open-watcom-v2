#include "fail.h"

typedef unsigned __int64 UINT;

unsigned mp[] = {
    3,5,7,13,
    17,19,31,
};

UINT L[100];

// is 2^q-1 prime?
int isPrime( unsigned q ) {
    unsigned i;
    UINT mask;

    mask = 1;
    mask <<= q;
    --mask;
    L[0] = 4;
    for( i = 1; i <= (q-2); ++i ) {
        L[i] = ( L[i-1] * L[i-1] - 2 ) % mask;
    }
    return( L[q-2] == 0 );
}

int main() {
    int i;
    for( i = 0; i < sizeof(mp)/sizeof(mp[0]); ++i ) {
        if( !isPrime( mp[i] ) ) _fail;
    }
    _PASS;
}
