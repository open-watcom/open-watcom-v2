#include "fail.h"

#ifndef __INLINE_FUNCTIONS__
#define __INLINE_FUNCTIONS__
#endif
#include <stdio.h>
#if defined( M_I86 ) || defined( MI386 )
#    include <i86.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

void checki( long c, long v, unsigned line )
{
    if( c != v ) {
        fail( line );
    }
}

#if defined( M_I86 ) || defined( MI386 )
void never_execute()
{
    inp(0);
    inpw(0);
    outp(0,1);
    outpw(0,1);
}
#endif

#define BUFF_LEN 13

int main()
{
    auto char source[BUFF_LEN];
    auto char dest[BUFF_LEN];
    auto div_t div_ret;
    auto ldiv_t ldiv_ret;

    checki( abs(-1), 1, __LINE__ );
    checki( labs(-2L), 2, __LINE__ );
    checki( fabs(-2.0)/2.0, 1, __LINE__ );
#if defined( M_I86 ) || defined( MI386 )
    memset( source, '!', BUFF_LEN );
    memset( dest, '@', BUFF_LEN );
    movedata( FP_SEG(source), FP_OFF(source), FP_SEG(dest), FP_OFF(dest), BUFF_LEN );
    checki( dest[0] == '!', 1, __LINE__ );
#endif

#if 0 // will not work under OS/2
    _enable();
    _disable();
#endif

    checki( _rotl( 1, 2 ), 4, __LINE__ );
    checki( _rotr( 4, 2 ), 1, __LINE__ );
    div_ret = div( 7, 3 );
    checki( div_ret.quot == 2 && div_ret.rem == 1, 1, __LINE__ );
    ldiv_ret = ldiv( 7, 3 );
    checki( ldiv_ret.quot == 2 && ldiv_ret.rem == 1, 1, __LINE__ );
#if defined( M_I86 ) || defined( MI386 )
    _fstrcpy( source, "!" );
    _fstrcat( source, "!" );
    checki( _fstrcmp( source, "!!" ) == 0, 1, __LINE__ );
    checki( _fstrlen( source ) == 2, 1, __LINE__ );
    _fmemset( source, '#', BUFF_LEN );
    source[10] = '$';
    checki( _fmemchr( source, '$', BUFF_LEN ) == &source[10], 1, __LINE__ );
    _fmemcpy( dest, source, BUFF_LEN );
    checki( _fmemcmp( source, dest, BUFF_LEN ) == 0, 1, __LINE__ );
#endif
    strcpy( source, "!" );
    strcat( source, "!" );
    checki( strcmp( source, "!!" ) == 0, 1, __LINE__ );
    checki( strlen( source ) == 2, 1, __LINE__ );
    memset( source, '#', BUFF_LEN );
    source[10] = '$';
    checki( memchr( source, '$', BUFF_LEN ) == &source[10], 1, __LINE__ );
    memcpy( dest, source, BUFF_LEN );
    checki( memcmp( source, dest, BUFF_LEN ) == 0, 1, __LINE__ );
    strcpy( source, "123456" );
    checki( strchr( source, '3' ) == &source[2], 1, __LINE__ );
    _PASS;
}
