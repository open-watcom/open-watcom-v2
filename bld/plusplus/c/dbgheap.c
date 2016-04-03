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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "plusplus.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __WATCOMC__
#include <malloc.h>
#include <process.h>
#endif
#include "bool.h"
#include "dbg.h"


typedef struct                  // ALLOCED -- allocated entry
{   void *_pentry;                  // - entry
    size_t _size;               // - size
} ALLOCED;

#if 0
static ALLOCED* entries;        // - allocated entries
static unsigned entry_count;    // - # entries
#endif


void DbgHeapInit                // INITIALIZATION
    ( void )
{
#if 0
    unsigned count;             // - # of entries
    _HEAPINFO hi;               // - heap information
    ALLOCED* ae;                // - allocated entry

    hi._pentry = NULL;
    count = 0;
    for( ; ; ) {
        switch( _heapwalk( &hi ) ) {
          case _HEAPEND :
          case _HEAPEMPTY :
            break;
          case _HEAPOK :
            if( hi._useflag ) {
                ++ count;
            }
            continue;
          case _HEAPBADBEGIN :
            puts( "DbgHeapInit -- BAD BEGIN" );
            abort();
          case _HEAPBADPTR :
            puts( "DbgHeapInit -- BAD PTR" );
            abort();
          case _HEAPBADNODE :
            puts( "DbgHeapInit -- BAD NODE" );
            abort();
        }
        break;
    }
    if( count == 0 ) {
        entries = NULL;
        entry_count = 0;
    } else {
        ++ count;
        entry_count = count;
        ae = malloc( sizeof( ALLOCED ) * count );
        if( ae == NULL ) {
            puts( "DbgHeapInit -- cannot initialize" );
            abort();
        }
        entries = ae;
        hi._pentry = NULL;
        count = 0;
        for( ; ; ) {
            switch( _heapwalk( &hi ) ) {
              case _HEAPEND :
              case _HEAPEMPTY :
                break;
              case _HEAPOK :
                if( hi._useflag ) {
                    ++ count;
                    ae->_pentry = hi._pentry;
                    ae->_size = hi._size;
                    ++ae;
                }
                continue;
              case _HEAPBADBEGIN :
                puts( "DbgHeapInit -- BAD BEGIN" );
                abort();
              case _HEAPBADPTR :
                puts( "DbgHeapInit -- BAD PTR" );
                abort();
              case _HEAPBADNODE :
                puts( "DbgHeapInit -- BAD NODE" );
                abort();
            }
            break;
        }
        if( count != entry_count ) {
            puts( "DbgHeapInit -- count mismatch" );
            abort();
        }
    }
#endif
}


void DbgHeapFini                // COMPLETION
    ( void )
{
#if 0
    unsigned count;             // - # of entries
    _HEAPINFO hi;               // - heap information
    ALLOCED* ae;                // - allocated entry

    hi._pentry = NULL;
    count = 0;
    for( ; ; ) {
        switch( _heapwalk( &hi ) ) {
          case _HEAPEND :
          case _HEAPEMPTY :
            break;
          case _HEAPOK :
            if( hi._useflag ) {
                unsigned dec;
                for( ae = entries, dec = entry_count; ; --dec, ++ae ) {
                    if( dec == 0 ) {
                        printf( "DbgHeapFini -- unfreed entry %p size %x\n"
                              , hi._pentry
                              , hi._size );
                        break;
                    }
                    if( hi._pentry == ae->_pentry ) {
                        ++ count;
                        break;
                    }
                }
            }
            continue;
          case _HEAPBADBEGIN :
            puts( "DbgHeapFini -- BAD BEGIN" );
            abort();
          case _HEAPBADPTR :
            puts( "DbgHeapFini -- BAD PTR" );
            abort();
          case _HEAPBADNODE :
            puts( "DbgHeapFini -- BAD NODE" );
            abort();
        }
        break;
    }
    if( count != entry_count ) {
        puts( "DbgHeapFini -- freed entries since DbgHeapInit" );
    }
    if( NULL != entries ) {
        free( entries );
    }
#endif
}
