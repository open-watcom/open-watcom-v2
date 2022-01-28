/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Macro dump utility.
*
****************************************************************************/


#include "_preproc.h"
#include "ppdmpmac.h"


static void dumpheap( void )
/**************************/
{
#if 0
    struct _heapinfo h;
    int         status;

    h._pentry = NULL;
    for( ;; ) {
        status = _heapwalk( &h );
        if( status != _HEAPOK )
            break;
        printf( "%s block at %Fp of size %4.4X\n",
                (h._useflag == _USEDENTRY ? "USED": "FREE"),
                h._pentry, h._size );
    }
    switch( status ) {
    case _HEAPEND:
        printf( "OK - end of heap\n" );
        break;
    case _HEAPEMPTY:
        printf( "OK - heap is empty\n" );
        break;
    case _HEAPBADBEGIN:
        printf( "ERROR - heap is damaged\n" );
        break;
    case _HEAPBADPTR:
        printf( "ERROR - bad pointer to heap\n" );
        break;
    case _HEAPBADNODE:
        printf( "ERROR - bad node in heap\n" );
        break;
    }
#endif
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    /* unused parameters */ (void)p;

    return( 1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    int         c;
    int         rc;

    dumpheap();
    if( argc < 2 ) {
        printf( "Usage: dumpmac filename\n" );
        exit( 1 );
    }
    PP_Init( ( argv[2] != NULL ) ? argv[2][0] : '#' );
    if( PP_FileInit( argv[1], PPFLAG_NONE | PPFLAG_TRUNCATE_FILE_NAME, NULL ) != 0 ) {
        PP_Fini();
        printf( "Unable to open '%s'\n", argv[1] );
        exit( 1 );
    }
    for( ;; ) {
        c = PP_Char();
        if( c == EOF )
            break;
//      putchar( c );
    }
    PP_Dump_Macros();
    dumpheap();
    PP_FileFini();
    rc = PP_Fini();
    dumpheap();
    return( ( rc ) ? EXIT_FAILURE : EXIT_SUCCESS );
}
