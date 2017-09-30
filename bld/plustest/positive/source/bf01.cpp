#include "fail.h"

int count;

struct s {
    unsigned int i : 2;
    unsigned int j : 3;
    unsigned int k : 4;
};

s x,y,z;

int main()
{
    int m = 1;

    (m ? x.i : x.j) = 1;
    ((!m) ? y.j : y.k) = 1;
    (m ? z.k : z.i) = 1;
    if( x.i != 1 || x.j != 0 || x.k != 0 ) fail( __LINE__ );
    if( y.i != 0 || y.j != 0 || y.k != 1 ) fail( __LINE__ );
    if( z.i != 0 || z.j != 0 || z.k != 1 ) fail( __LINE__ );
    _PASS;
}
