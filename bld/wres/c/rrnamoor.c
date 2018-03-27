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
#include "layer0.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

ResNameOrOrdinal *ResReadNameOrOrdinal( FILE *fp )
/************************************************/
{
    uint_8              flags;
    uint_16             ord;
    ResNameOrOrdinal    *newptr;
    bool                error;
    size_t              stringlen;
    char                *restofstr;

    if( ResReadUint8( &flags, fp ) )
        return( NULL );

    /* read the rest of the Name or Ordinal */
    ord = 0;
    restofstr = NULL;
    stringlen = 0;
    if( flags == 0xff ) {
        error = ResReadUint16( &ord, fp );
    } else if( flags == 0 ) {
        error = false;
    } else {
        restofstr = ResReadString( fp, &stringlen );
        stringlen += 1; /* for the '\0' */
        error = ( restofstr == NULL );
    }

    /* allocate space for the new Name or Ordinal */
    newptr = NULL;
    if( !error ) {
        newptr = WRESALLOC( sizeof( ResNameOrOrdinal ) + stringlen );
        if( newptr == NULL ) {
            error = WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the new new Name or Ordinal into the correct place */
    if( !error ) {
        newptr->ord.fFlag = flags;
        if( flags == 0xff ) {
            newptr->ord.wOrdinalID = ord;
        } else {
            if( restofstr != NULL ) {
                memcpy( newptr->name + 1, restofstr, stringlen );
            }
        }
    }

    if( restofstr != NULL ) {
        WRESFREE( restofstr );
    }

    return( newptr );
}

ResNameOrOrdinal *ResRead32NameOrOrdinal( FILE *fp )
/**************************************************/
{
    uint_16             flags;
    uint_16             ord;
    ResNameOrOrdinal    *newptr;
    bool                error;
    size_t              stringlen;
    char                *restofstr;

    if( ResReadUint16( &flags, fp ) )
        return( NULL );

    /* read the rest of the Name or Ordinal */
    ord = 0;
    restofstr = NULL;
    stringlen = 0;
    if( flags == 0xffff ) {
        error = ResReadUint16( &ord, fp );
    } else if( flags == 0 ) {
        error = false;
    } else {
        restofstr = ResRead32String( fp, &stringlen );
        stringlen += 1; /* for the '\0' */
        error = ( restofstr == NULL );
    }

    /* allocate space for the new Name or Ordinal */
    newptr = NULL;
    if( !error ) {
        newptr = WRESALLOC( sizeof( ResNameOrOrdinal ) + stringlen );
        if( newptr == NULL ) {
            error = WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the new Name or Ordinal into the correct place */
    if( !error ) {
        newptr->ord.fFlag = flags;
        if( flags == 0xffff ) {
            newptr->ord.wOrdinalID = ord;
        } else {
            if( restofstr != NULL ) {
                memcpy( newptr->name + 1, restofstr, stringlen );
            }
        }
    }


    if( restofstr != NULL ) {
        WRESFREE( restofstr );
    }

    return( newptr );
}
