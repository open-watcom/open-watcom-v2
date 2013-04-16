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
* Description:  Setup memory management functions.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"
#include "standard.h"
#include "utmem.h"
#include "utmemdbg.h"

#if DMEM_OVERRUNS
    extern unsigned     MaxAllocSize;
#endif

void ut_fatal( char *s )
{
    GUIDisplayMessage( NULL, s, " Memory ERROR ", GUI_OK ); // nyi - kanji
    exit( 1 );
}

void ut_out_of_memory( char *s )
{
    s = s;
}

extern void *GUIAlloc( unsigned size )
{
    return( ut_alloc( size ) );
}

extern void GUIFree( void *chunk )
{
    ut_free( chunk );
}

extern void *GUIRealloc( void * chunk, unsigned size )
{
    return( ut_realloc( chunk, size ) );
}

extern void *bdiff_malloc( size_t size )
{
    return( ut_alloc( size ) );
}

extern void bdiff_free( void *chunk )
{
    ut_free( chunk );
}

extern void *bdiff_realloc( void * chunk, size_t size )
{
    return( ut_realloc( chunk, size ) );
}

extern void GUIMemOpen()
{
#if DMEM_OVERRUNS
#ifdef _M_I86
    MaxAllocSize = 0xfff8;
#else
    MaxAllocSize = 0xfffffff8;
#endif
#endif
}

extern void GUIMemClose( void )
{
}

extern void GUIMemPrtUsage( void )
{
}

