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


#include "precomp.h"
#include <string.h>

#include "wdeglbl.h"
#include "wdemem.h"
#include "wdedebug.h"
#include "wdelist.h"

void WdeInsertObject( LIST **list, void *obj )
{
    LIST *end;

    if( list == NULL ) {
        WdeWriteTrail( "WdeInsertObject: bad list var!" );
        return;
    }

    if( *list == NULL ) {
        ListAddElt( list, obj );
    } else {
        /* make sure obj is inserted at end of list */
        WdeListLastElt( *list, &end );
        ListInsertElt( end, obj );
    }
}

void WdeListLastElt( LIST *list, LIST **last )
{
    LIST *end;

    if( list == NULL || last == NULL ) {
        if( last != NULL ) {
            *last = NULL;
        }
        return;
    }

    /* find the end of the list */
    for( end = list; end != NULL && ListNext( end ) != NULL; end = ListNext( end ) );

    *last = end;
}

Bool WdeListConcat( LIST **dest, LIST *src, uint_32 size )
{
    LIST    *end;
    LIST    *olist;
    void    *elt;

    if( dest == NULL ) {
        return( TRUE );
    }

    WdeListLastElt( *dest, &end );

    for( olist = src; olist != NULL; olist = ListNext( olist ) ) {
        if( size == 0 ) {
            elt = ListElement( olist );
        } else {
            elt = WdeMemAlloc( size );
            if( elt == NULL ) {
                return( FALSE );
            }
            memcpy( elt, ListElement( olist ), size );
        }
        if( end == NULL ) {
            ListAddElt( dest, elt );
            end = *dest;
        } else {
            ListInsertElt( end, elt );
            end = ListNext( end );
        }
    }

    return( TRUE );
}

LIST *WdeListCopy( LIST *src )
{
    LIST    *new;
    LIST    *end;
    LIST    *olist;

    end = NULL;
    new = NULL;

    for( olist = src; olist != NULL; olist = ListNext( olist ) ) {
        if( end == NULL ) {
            ListAddElt( &new, ListElement( olist ) );
            end = new;
        } else {
            ListInsertElt( end, ListElement( olist ) );
            end = ListNext( end );
        }
    }

    return( new );
}
