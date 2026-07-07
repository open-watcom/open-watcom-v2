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

M32ResResourceHeader *M32ResReadResourceHeader( FILE *fp )
/********************************************************/
{
    M32ResResourceHeader    *newhead;
    bool                    error;

    error = false;
    newhead = WRESALLOC( sizeof( M32ResResourceHeader ) );
    if( newhead == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
        return( NULL );
    }
    newhead->head16 = WRESALLOC( sizeof( MResResourceHeader ) );
    if( newhead->head16 == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
        WRESFREE( newhead );
        return( NULL );
    }
    error = ResReadPadDWord( fp );
    if( !error ) {
        newhead->head16->Size = ResReadUint32( &error, fp );
    }
    if( !error ) {
        newhead->Size = ResReadUint32( &error, fp );
    }
    if( !error ) {
        newhead->head16->Type = ResRead32NameOrOrdinal( fp );
        error = (newhead->head16->Type == NULL );
    }
    if( !error ) {
        newhead->head16->Name = ResRead32NameOrOrdinal( fp );
        error = (newhead->head16->Name == NULL );
    }
    if( !error ) {
        error = ResReadPadDWord( fp );
    }
    if( !error ) {
        newhead->head16->DataVersion = ResReadUint32( &error, fp );
    }
    if( !error ) {
        newhead->head16->MemoryFlags = ResReadUint16( &error, fp );
    }
    if( !error ) {
        newhead->head16->LanguageId = ResReadUint16( &error, fp );
    }
    if( !error ) {
        newhead->head16->Version = ResReadUint32( &error, fp );
    }
    if( !error ) {
        newhead->head16->Characteristics = ResReadUint32( &error, fp );
    }
    if( error ) {
        WRESFREE( newhead->head16 );
        WRESFREE( newhead );
        newhead = NULL;
    }

    return( newhead );
}

MResResourceHeader *MResReadResourceHeader( FILE *fp )
/****************************************************/
{
    MResResourceHeader      *newhead;
    bool                    error;

    newhead = WRESALLOC( sizeof( MResResourceHeader ) );
    if( newhead == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
        return( NULL );
    }
    newhead->Type = ResReadNameOrOrdinal( fp );
    if( newhead->Type != NULL ) {
        newhead->Name = ResReadNameOrOrdinal( fp );
        if( newhead->Name != NULL ) {
            error = false;
            newhead->MemoryFlags = ResReadUint16( &error, fp );
            if( !error ) {
                newhead->Size = ResReadUint32( &error, fp );
                if( !error ) {
                    return( newhead );
                }
            }
        }
    }
    WRESFREE( newhead );
    return( NULL );
} /* MResReadResourceHeader */
