/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include "layer0.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"


void *ResReadWResIDName( unsigned offs, FILE *fp, uint_16 ver )
/*************************************************************/
{
    WResIDName      *newptr;
    size_t          numread;
    uint_16         numchars;
    char            *ptr;

    /* read the size of the name in */
    if( ver < 3 ) {
        uint_8  tmp;

        if( (numread = WRESREAD( fp, &tmp, sizeof( tmp ) )) != sizeof( tmp ) ) {
            WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
            return( NULL );
        }
        numchars = tmp;
    } else {
        if( (numread = WRESREAD( fp, &numchars, sizeof( numchars ) )) != sizeof( numchars ) ) {
            WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
            return( NULL );
        }
    }

    /* alloc the space for the new record */
    /* -1 because one of the chars in the name is declared in the struct */
    ptr = WRESALLOC( offs + sizeof( WResIDName ) - 1 + numchars );
    if( ptr == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newptr = (WResIDName *)( ptr + offs );
        /* read in the characters */
        newptr->NumChars = numchars;
        if( (numread = WRESREAD( fp, newptr->Name, numchars )) != numchars ) {
            WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
            WRESFREE( ptr );
            ptr = NULL;
        }
    }

    return( ptr );
}

WResIDName *WResReadWResIDName( FILE *fp )
/****************************************/
{
    return( ResReadWResIDName( 0, fp, WRESVERSION ) );
}

