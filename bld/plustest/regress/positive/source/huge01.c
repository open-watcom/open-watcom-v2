#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#ifdef _M_IX86
#if _M_IX86 < 300

#if __WATCOMC__ > 1060
#define N 4096

typedef struct temp {
    short a[32];
} temp;

struct temp huge HugeArr[N];
struct temp huge *HugeMem;

void barf( char *a, int i, unsigned line )
{
    printf( __FILE__ ": incorrect huge array access %s[%i]\n", a, i );
    fail( line );
}

void check( int i )
{
    if( HugeArr[i].a[0] != 1 ) barf( "HugeArr", i, __LINE__ );
    if( HugeArr[i].a[1] != i ) barf( "HugeArr", i, __LINE__ );
    if( HugeArr[i].a[2] != 99 ) barf( "HugeArr", i, __LINE__ );
    if( HugeArr[i].a[31] != 99 ) barf( "HugeArr", i, __LINE__ );
    if( HugeMem[i].a[0] != 1 ) barf( "HugeMem", i, __LINE__ );
    if( HugeMem[i].a[1] != i ) barf( "HugeMem", i, __LINE__ );
    if( HugeMem[i].a[2] != 99 ) barf( "HugeMem", i, __LINE__ );
    if( HugeMem[i].a[31] != 99 ) barf( "HugeMem", i, __LINE__ );
}

int main()
{
    int i;
    temp localvar;

    HugeMem = (temp huge *) halloc( N, sizeof(temp) );
    if( HugeMem == NULL ) fail(__LINE__);
    for( i = 0; i < 32; ++i ) {
        localvar.a[i]  = 99;
    }
    localvar.a[0] = 1;

    for( i = 0; i < N; ++i ){
        localvar.a[1] = i;
        HugeArr[i]=localvar;
        HugeMem[i]=localvar;
	check( i );
	if( errors > 16 ) break;
    }
    for( i = 0; i < N; i += 16 ){
	check( i );
	if( errors > 16 ) break;
    }
    _PASS;
}
#else
int main() {
    _PASS;
}
#endif
#else
int main() {
    _PASS;
}
#endif
#else
int main() {
    _PASS;
}
#endif
