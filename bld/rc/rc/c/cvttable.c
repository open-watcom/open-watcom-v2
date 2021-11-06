/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  conversion tool to convert Japanese text files
*               generate table for UTF-8 -> Shift-JIS Windows CP932
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "cvttable.h"


typedef int (*comp_fn)(const void *,const void *);

static cvt_chr cvt_table[] = {
    #define pickb(s,u) {s, u},
    #define picki(s,u)
    #include "cp932uni.h"
    #undef picki
    #undef pickb
};

#if 0
static int compare_sjis( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->s - p2->s );
}
#endif

static int compare_utf8( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->u - p2->u );
}

static void usage( void )
{
    printf( "Usage: cvttable <output file>\n" );
}

int main( int argc, char *argv[] )
{
    FILE            *fo;
    unsigned short  tablen;

    /* unused parameters */ (void)argc;

    ++argv;
    if( *argv == NULL ) {
        usage();
        printf( "\nMissing output file name.\n" );
        return( 4 );
    }
    fo = fopen( *argv, "wb" );
    if( fo == NULL ) {
        printf( "Cannot open output file '%s'.\n", *argv );
        return( 5 );
    }
    tablen = sizeof( cvt_table ) / sizeof( cvt_table[0] );
    qsort( cvt_table, tablen, sizeof( cvt_table[0] ), (comp_fn)compare_utf8 );
    fwrite( &tablen, sizeof( tablen ), 1, fo );
    fwrite( cvt_table, sizeof( cvt_chr ), tablen, fo );
    fclose( fo );
    return( 0 );
}
