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

ResNameOrOrdinal *ResReadNameOrOrdinal( WResFileID handle )
/*********************************************************/
{
    ResNameOrOrdinal    newname;
    ResNameOrOrdinal *  newptr;
    bool                error;
    int                 stringlen;
    char *              restofstr;
    uint_8              tmp8;
    uint_16             tmp16;

    restofstr = NULL;
    error = ResReadUint8( &tmp8, handle );
    newname.ord.fFlag = tmp8;
    newname.ord.wOrdinalID = 0;
    stringlen = 0;

    /* read the rest of the Name or Ordinal */
    if( !error ) {
        if( newname.ord.fFlag == 0xff ) {
            error = ResReadUint16( &tmp16, handle );
            newname.ord.wOrdinalID = tmp16;
        } else {
            if( newname.name[0] != '\0' ) {
                restofstr = ResReadString( handle, &stringlen );
                stringlen += 1; /* for the '\0' */
                error = (restofstr == NULL);
            }
        }
    }

    /* allocate space for the new Name or Ordinal */
    if( error ) {
        newptr = NULL;
    } else {
        newptr = WRESALLOC( sizeof( ResNameOrOrdinal ) + stringlen );
        error = (newptr == NULL);
        if( error ) WRES_ERROR( WRS_MALLOC_FAILED );
    }

    /* copy the new new Name or Ordinal into the correct place */
    if( !error ) {
        newptr->ord.fFlag = newname.ord.fFlag;
        if( newname.ord.fFlag == 0xff ) {
            newptr->ord.wOrdinalID = newname.ord.wOrdinalID;
        } else {
            if( newptr->name[0] != '\0' ) {
                memcpy( &(newptr->name[1]), restofstr, stringlen );
                WRESFREE( restofstr );
            }
        }
    }

    return( newptr );
}

ResNameOrOrdinal *ResRead32NameOrOrdinal( WResFileID handle )
/***********************************************************/
{
    uint_16             flags;
    uint_16             ord;
    ResNameOrOrdinal *  newptr;
    bool                error;
    int                 stringlen;
    char *              restofstr;

    restofstr = NULL;
    ord = 0;
    stringlen = 0;

    error = ResReadUint16( &flags, handle );
    /* read the rest of the Name or Ordinal */
    if( !error ) {
        if( flags == 0xffff ) {
            error = ResReadUint16( &ord, handle );
        } else {
            if( flags != 0x0000 ) {
                restofstr = ResRead32String( handle, &stringlen );
                stringlen += 1; /* for the '\0' */
                error = (restofstr == NULL);
            }
        }
    }

    /* allocate space for the new Name or Ordinal */
    if( error ) {
        newptr = NULL;
    } else {
        newptr = WRESALLOC( sizeof( ResNameOrOrdinal ) + stringlen );
        error = (newptr == NULL);
        if( error ) WRES_ERROR( WRS_MALLOC_FAILED );
    }

    /* copy the new new Name or Ordinal into the correct place */
    if( !error ) {
        if( flags == 0xffff ) {
            newptr->ord.fFlag = 0xff;
            newptr->ord.wOrdinalID = ord;
        } else {
            newptr->name[0] = (char)(flags & 0x00ff);
            if( flags != 0x0000 ) {
                memcpy( &(newptr->name[1]), restofstr, stringlen );
                WRESFREE( restofstr );
            }
        }
    }

    return( newptr );
}
