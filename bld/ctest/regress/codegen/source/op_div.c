#include "fail.h"
#include <limits.h>

#include "cgtypes.h"

#define ONE_FN( T1, T2, T3 )    T1 op_div_##T1##_##T2##_##T3 ( T2 a, T3 b ) \
                                    { return( a / b ); }

#include "bin_i_f.h"

GEN_TESTS

int main( void )
{
    _PASS;
}
