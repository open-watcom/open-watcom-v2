#include "fail.h"
#include <stddef.h>

#ifdef __386__
#define SIZE_FAR
#define SIZE_HUGE
#else
#define SIZE_FAR far
#define SIZE_HUGE huge
#endif

char SIZE_FAR two[60u*1024];
char SIZE_HUGE four[120L*1024];
size_t check_two = sizeof( size_t );
size_t check_four = sizeof( unsigned long );

main()
{
    if( sizeof sizeof two != check_two ) fail(__LINE__);
    if( sizeof sizeof four != check_four ) fail(__LINE__);
    _PASS;
}
