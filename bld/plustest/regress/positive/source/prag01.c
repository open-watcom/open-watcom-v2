#include "fail.h"

#if ( __WATCOMC__ > 950 ) && ( defined( M_I86 ) || defined( M_I386 ) )

int one_cpp( int x, int y = -1 )
{
    return x - y;
}
#pragma aux one_cpp parm reverse;

int one_cpp_one_c( int x, int y )
{
    return x - y;
}
extern "C" int one_cpp_one_c( char x, char y )
{
    return x - y;
}
#pragma aux one_cpp_one_c parm reverse;

int two_cpps_one_c( int x, int y )
{
    return x - y;
}
int two_cpps_one_c( char x, char y )
{
    return x - y;
}
extern "C" int two_cpps_one_c( char x, int y )
{
    return x - y;
}
#pragma aux two_cpps_one_c parm reverse;

extern "C" int template_one_c( char x, char y )
{
    return x - y;
}
template <class T>
    T template_one_c( T x, T y )
    {
        return x - y;
    }
#pragma aux template_one_c parm reverse;

int main()
{
    if( one_cpp( 1 ) != ( 1 - -1 ) ) fail(__LINE__);
    if( one_cpp( 2, 1 ) != ( 2 - 1 ) ) fail(__LINE__);
    if( one_cpp_one_c( 1, 2 ) != ( 1 - 2 ) ) fail(__LINE__);
    if( one_cpp_one_c( '2', '1' ) != ( '2' - '1' ) ) fail(__LINE__);
    if( two_cpps_one_c( 1, 2 ) != ( 1 - 2 ) ) fail(__LINE__);
    if( two_cpps_one_c( '1', '2' ) != ( '1' - '2' ) ) fail(__LINE__);
    if( two_cpps_one_c( '2', 1 ) != ( '2' - 1 ) ) fail(__LINE__);
    if( template_one_c( '2', '1' ) != ( '2' - '1' ) ) fail(__LINE__);
    if( template_one_c( 1, 2 ) != ( 1 - 2 ) ) fail(__LINE__);
    _PASS;
}

#else

ALWAYS_PASS

#endif
