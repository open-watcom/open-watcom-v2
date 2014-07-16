#include "fail.h"

struct S {
    unsigned short arr[16];
    unsigned sum;

    S() {
        unsigned short i;
        sum = 0;
        for( i = 0; i < 16; ++ i ) {
            arr[i] = i;
            sum += i;
        }
    }
    
};

struct U {
    int u;
};

struct T : U, S {
};

struct V : virtual T {};

unsigned short sum( V const & s )
{
    unsigned short total;
    int i;
    total = 0;
    for( i = 0; i < 16; ++i ) {
        total += s.arr[i];
    }
    if( total != s.sum ) {
        fail( __LINE__ );
    }
    return total;
}

int main()
{
    V vv;
    unsigned short total;

    total = sum( vv );
    if( total != vv.sum ) {
        fail( __LINE__ );
    }
    _PASS;
}
