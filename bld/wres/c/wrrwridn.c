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
#include <stddef.h>
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"


void *AllocWResIDName( unsigned offs, unsigned numchars )
/*******************************************************/
{
    char            *ptr;

    ptr = WRESALLOC( offs + offsetof( WResIDName, Name ) + numchars );
    if( ptr == NULL )
        WRES_ERROR( WRS_MALLOC_FAILED );
    return( ptr );
}

void *ResReadWResIDName( unsigned offs, FILE *fp, uint_16 ver )
/*************************************************************/
{
    size_t          numread;
    uint_16         numchars;
    char            *ptr;
    WResIDName      *idname;
    bool            error;

    error = false;
    /* read the size of the name in */
    if( ver < 3 ) {
        numchars = ResReadUint8( &error, fp );
    } else {
        numchars = ResReadUint16( &error, fp );
    }
    if( error )
        return( NULL );

    /* alloc the space for the new record */
    /* -1 because one of the chars in the name is declared in the struct */
    ptr = AllocWResIDName( offs, numchars );
    if( ptr != NULL ) {
        idname = (WResIDName *)( ptr + offs );
        idname->NumChars = numchars;
        /* read in the characters */
        if( (numread = WRESREAD( fp, idname->Name, numchars )) != numchars ) {
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

