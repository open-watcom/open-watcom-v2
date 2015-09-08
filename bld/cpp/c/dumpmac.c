/****************************************************************************
*
*                            Open Watcom Project
*
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


#include "preproc.h"
#include "ppdmpmac.h"


static void dumpheap( void )
{
#if 0
    struct _heapinfo h;
    int         status;

    h._pentry = NULL;
    for( ;; ) {
        status = _heapwalk( &h );
        if( status != _HEAPOK )
            break;
        printf( "%s block at %Fp of size %4.4X\r\n",
                (h._useflag == _USEDENTRY ? "USED": "FREE"),
                h._pentry, h._size );
    }
    switch( status ) {
    case _HEAPEND:
        printf( "OK - end of heap\r\n" );
        break;
    case _HEAPEMPTY:
        printf( "OK - heap is empty\r\n" );
        break;
    case _HEAPBADBEGIN:
        printf( "ERROR - heap is damaged\r\n" );
        break;
    case _HEAPBADPTR:
        printf( "ERROR - bad pointer to heap\r\n" );
        break;
    case _HEAPBADNODE:
        printf( "ERROR - bad node in heap\r\n" );
        break;
    }
#endif
}

int main( int argc, char *argv[] )
{
    int         c;

    dumpheap();
    if( argc < 2 ) {
        printf( "Usage: dumpmac filename\r\n" );
        exit( 1 );
    }
    if( argv[2] != NULL ) {
        PreProcChar = argv[2][0];
    }
    if( PP_Init( argv[1], 0, NULL ) != 0 ) {
        printf( "Unable to open '%s'\r\n", argv[1] );
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
    PP_Fini();
    dumpheap();
    return( 0 );
}
