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


#include "wio.h"
#include "global.h"
#include "semantic.h"
#include "errors.h"
#include "reserr.h"
#include "depend.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"

#define BUFFER_SIZE     1024

static int copyAResource( WResFileID handle, WResDirWindow *wind,
                            char *buffer, const char *filename )
/***********************************************************************/
{
    ResLocation         loc;
    WResLangInfo        *langinfo;
    WResTypeInfo        *typeinfo;
    WResResInfo         *resinfo;
//    RcStatus            rc;
    int                 err_code;

    langinfo = WResGetLangInfo( *wind );
    resinfo = WResGetResInfo( *wind );
    typeinfo = WResGetTypeInfo( *wind );
    loc.start = SemStartResource();
    /* rc = */ CopyData( langinfo->Offset, langinfo->Length, handle, buffer, BUFFER_SIZE, &err_code );
    loc.len = SemEndResource( loc.start );
    SemAddResource2( &resinfo->ResName, &typeinfo->TypeName, langinfo->MemoryFlags, loc, filename );
    return( FALSE );
}

static int copyResourcesFromRes( const char *full_filename )
/**********************************************************/
{
    WResFileID          handle;
    WResDir             dir;
    int                 dup_discarded;
    WResDirWindow       wind;
    char                *buffer;
    int                 error;

    buffer = NULL;
    dir = WResInitDir();
    handle = RcIoOpenInput( full_filename, O_RDONLY | O_BINARY );
    if (handle == NIL_HANDLE) {
        RcError( ERR_CANT_OPEN_FILE, full_filename, strerror( errno ) );
        goto HANDLE_ERROR;
    }

    error = WResReadDir( handle, dir, &dup_discarded );
    if( error ) {
        switch( LastWresStatus() ) {
        case WRS_BAD_SIG:
            RcError( ERR_INVALID_RES, full_filename );
            break;
        case WRS_BAD_VERSION:
            RcError( ERR_BAD_RES_VER, full_filename );
            break;
        default:
            RcError( ERR_READING_RES, full_filename, LastWresErrStr() );
            break;
        }
        goto HANDLE_ERROR;
    }

    if( WResGetTargetOS( dir ) != WResGetTargetOS( CurrResFile.dir ) ) {
        RcError( ERR_RES_OS_MISMATCH, full_filename );
        goto HANDLE_ERROR;
    }
    buffer = RCALLOC( BUFFER_SIZE );
    wind = WResFirstResource( dir );
    while( !WResIsEmptyWindow( wind ) ) {
        copyAResource( handle, &wind, buffer, full_filename );
        wind = WResNextResource( wind, dir );
    }
    RCFREE( buffer );
    WResFreeDir( dir );
    RCCLOSE( handle );
    return( FALSE );

HANDLE_ERROR:
    ErrorHasOccured = TRUE;
    WResFreeDir( dir );
    if( handle != NIL_HANDLE )
        RCCLOSE( handle );
    if( buffer != NULL )
        RCFREE( buffer );
    return( TRUE );
}

void SemWINAddResFile( char *filename )
/*************************************/
{
    char                full_filename[ _MAX_PATH ];

    if( RcFindResource( filename, full_filename ) == -1 ) {
        RcError( ERR_CANT_FIND_FILE, filename );
        goto HANDLE_ERROR;
    }
    if( AddDependency( full_filename ) ) goto HANDLE_ERROR;
    if( copyResourcesFromRes( full_filename ) ) goto HANDLE_ERROR;
    RCFREE( filename );
    return;

HANDLE_ERROR:
    ErrorHasOccured = TRUE;
    RCFREE( filename );
}
