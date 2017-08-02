#include "pch09.h"

int main() {
#ifdef __WATCOM_INT64__
    if( doo( 1 ) > (-LONG_MIN-33) ) fail(__LINE__);
    if( doo( 9 ) != 1 ) fail(__LINE__);
    if( _CD_count != 2 ) fail(__LINE__);
#endif
    _PASS;
}
