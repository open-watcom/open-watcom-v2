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


static cvt_chr cvt_table[] = {
    #define pick(s,u) {s, u },
    #include "cp932uni.h"
    #undef pick
};

static int compare_sjis( const void *p1, const void *p2 )
{
    return( ((cvt_chr *)p1)->s - ((cvt_chr *)p2)->s );
}

static int compare_utf8( const void *p1, const void *p2 )
{
    return( ((cvt_chr *)p1)->u - ((cvt_chr *)p2)->u );
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
    qsort( cvt_table, tablen, sizeof( cvt_table[0] ), compare_utf8 );
    fwrite( &tablen, sizeof( tablen ), 1, fo );
    fwrite( cvt_table, sizeof( cvt_chr ), tablen, fo );
    fclose( fo );
    return( 0 );
}
