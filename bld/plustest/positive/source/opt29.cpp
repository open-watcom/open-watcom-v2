#include "fail.h"

#if __WATCOM_REVISION__ >= 8
#ifdef __I86__
#define _TFAR __far
#define _TSAVE __saveregs
void _trash_fs();
#pragma aux _trash_fs = "nop" modify [fs];
#else
#define _TFAR
#define _TSAVE
#define _trash_fs()
#endif

int a[] = { 1, 2, 3, 4, 5 };

unsigned f( int _TFAR *data1, int n, int _TFAR *data2 )
{
    if( data1 != a ) return(__LINE__);
    if( data2 != a ) return(__LINE__);
    if( data1[0] != 1 ) return(__LINE__);
    if( data2[1] != 2 ) return(__LINE__);
    if( data1[2] != 3 ) return(__LINE__);
    if( data2[3] != 4 ) return(__LINE__);
    if( data1[4] != 5 ) return(__LINE__);
    return 0;
}

unsigned doBuffer( int _TFAR *buffer )
{
    unsigned line;

    line = f( buffer, 1, buffer );
    if( line ) return(line);
    line = f( buffer, 1, buffer );
    if( line ) return(line);
    buffer[2] = 0;
    return 0;
}

unsigned _TSAVE db( int _TFAR *buffer ) {
    _trash_fs();
    unsigned line = doBuffer( buffer );
    _trash_fs();
    return line;
}

int main()
{
    unsigned line;
    line = db( a );
    if( line ) fail(line);
    _PASS;
}
#else
ALWAYS_PASS
#endif
