#include <stdlib.h>
#include "fnstat1.h"

extern int test( int *, int *, int *, int * );

int main()
{
    return test( ret_c1( 1 ), ret_c2( 2 ), ret_s1( 3 ), ret_s2( 4 ) );
}
