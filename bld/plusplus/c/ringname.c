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


#include <string.h>

#include "plusplus.h"
#include "ring.h"
#include "ringname.h"


char *RingNameAllocVct(         // ALLOCATE A NAME, FOR A VECTOR
    RINGNAMECTL *ctl,           // - control for ring
    char const *vector,         // - vector to be allocated
    size_t len )                // - length of vector
{
    RINGNAME *element;          // - new entry

    element = RingAlloc( &ctl->ring, offsetof( RINGNAME, name ) + len + 1 );
    stvcpy( element->name, vector, len );
    if( ! ctl->case_sensitive ) {
        strlwr( element->name );
    }
    return( element->name );
}


#if 0
RINGNAME *RingNameAllocStr(     // ALLOCATE A NAME, FOR A STRING
    RINGNAMECTL *ctl,           // - control for ring
    char *name )                // - name to be allocated
{
    return( RingNameAllocVct( ctl, name, strlen( name ) ) );
}
#endif


RINGNAME *RingNameLookup(       // LOOKUP A NAME
    RINGNAMECTL *ctl,           // - control for ring
    const char *name )          // - name to be looked up
{
    RINGNAME *curr;

    if( ctl->case_sensitive ) {
        RingIterBeg( ctl->ring, curr ) {
            if( strcmp( curr->name, name ) == 0 ) {
                return( curr );
            }
        } RingIterEnd( curr );
    } else {
        RingIterBeg( ctl->ring, curr ) {
            if( stricmp( curr->name, name ) == 0 ) {
                return( curr );
            }
        } RingIterEnd( curr );
    }
    return( NULL );
}

#if 0
unsigned RingNameLookupPosition(// LOOKUP A NAME'S POSITION (0-not found,base 1)
    RINGNAMECTL *ctl,           // - control for ring
    const char *name )          // - name to be looked up
{
    unsigned position;
    RINGNAME *curr;

    position = 1;
    if( ctl->case_sensitive ) {
        RingIterBeg( ctl->ring, curr ) {
            if( strcmp( curr->name, name ) == 0 ) {
                return( position );
            }
            ++position;
        } RingIterEnd( curr );
    } else {
        RingIterBeg( ctl->ring, curr ) {
            if( stricmp( curr->name, name ) == 0 ) {
                return( position );
            }
            ++position;
        } RingIterEnd( curr );
    }
    return( 0 );
}
#endif


void RingNameFree(              // FREE RING OF NAMES
    RINGNAMECTL *ctl )          // - control for ring
{
    RingFree( &ctl->ring );
}


void RingNameInit(              // INITIALIZE RING OF NAMES
    RINGNAMECTL *ctl )          // - control for ring
{
    ctl->ring = NULL;
}
