#include "fail.h"
#include <string.h>

#if __WATCOM_REVISION__ >= 8

typedef char Array[100];

void f(Array & arr) {
    if( arr[0] != '0' ) fail(__LINE__);
    if( arr[9] != '9' ) fail(__LINE__);
    arr[0] = '@';
    arr[9] = '@';
    if( sizeof( arr ) != 100 ) fail(__LINE__);
    for( int i = 0; i < sizeof( arr ); ++i ) {
	if( arr[i] != '@' ) fail(__LINE__);
    }
}

int main()
{
    Array a;
    memset( a, '@', sizeof( a ) );
    a[0] = '0';
    a[9] = '9';
    f(a);
    _PASS;
}
#else
ALWAYS_PASS
#endif
