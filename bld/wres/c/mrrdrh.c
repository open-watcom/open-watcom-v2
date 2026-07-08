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


MResResourceHeader *MResReadResourceHeader( FILE *fp, bool iswin32 )
/******************************************************************/
{
    MResResourceHeader      *msheader;
    bool                    error;

    error = false;
    msheader = WRESALLOC( sizeof( MResResourceHeader ) );
    if( msheader == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
        return( NULL );
    }
    if( iswin32 ) {
        error = ResReadPadDWord( fp );
        if( !error ) {
            msheader->Size = ResReadUint32( &error, fp );
        }
        if( !error ) {
            msheader->HdrSize = ResReadUint32( &error, fp );
        }
        if( !error ) {
            msheader->Type = ResRead32NameOrOrdinal( fp );
            error = ( msheader->Type == NULL );
        }
        if( !error ) {
            msheader->Name = ResRead32NameOrOrdinal( fp );
            error = ( msheader->Name == NULL );
        }
        if( !error ) {
            error = ResReadPadDWord( fp );
        }
        if( !error ) {
            msheader->DataVersion = ResReadUint32( &error, fp );
        }
        if( !error ) {
            msheader->MemoryFlags = ResReadUint16( &error, fp );
        }
        if( !error ) {
            msheader->LanguageId = ResReadUint16( &error, fp );
        }
        if( !error ) {
            msheader->Version = ResReadUint32( &error, fp );
        }
        if( !error ) {
            msheader->Characteristics = ResReadUint32( &error, fp );
        }
    } else {
        if( !error ) {
            msheader->Type = ResRead32NameOrOrdinal( fp );
            error = ( msheader->Type == NULL );
        }
        if( !error ) {
            msheader->Name = ResRead32NameOrOrdinal( fp );
            error = ( msheader->Name == NULL );
        }
        if( !error ) {
            msheader->MemoryFlags = ResReadUint16( &error, fp );
        }
        if( !error ) {
            msheader->Size = ResReadUint32( &error, fp );
        }
    }
    if( error ) {
        WRESFREE( msheader );
        msheader = NULL;
    }
    return( msheader );
}
