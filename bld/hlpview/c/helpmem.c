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
* Description:  Memory allocation/deallocation routines.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "helpmem.h"
#include "trmemcvr.h"


#ifdef TRMEM
static FILE     *memFP = NULL;
#endif

void HelpMemOpen( void )
{
#ifdef TRMEM
    TRMemOpen();
    memFP = fopen( "MEMERR", "w" );
    TRMemRedirect( memFP );
#endif
}

void HelpMemClose( void )
{
#ifdef TRMEM
    TRMemClose();
    if( ftell( memFP ) != 0 ) {
        printf( "***************************\n" );
        printf( "* A memory error occurred *\n" );
        printf( "***************************\n" );
    }
    fclose( memFP );
#endif
}

void HelpMemPrtList( void )
{
#ifdef TRMEM
    TRMemPrtList();
#endif
}

HELPMEM void *HelpMemAlloc( size_t size )
{
#ifdef TRMEM
    return( TRMemAlloc( size ) );
#else
    return( malloc( size ) );
#endif
}

HELPMEM void *HelpMemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( TRMemRealloc( ptr, size ) );
#else
    return( realloc( ptr, size ) );
#endif
}

HELPMEM void HelpMemFree( void *ptr )
{
#ifdef TRMEM
    TRMemFree( ptr );
#else
    free( ptr );
#endif
}
