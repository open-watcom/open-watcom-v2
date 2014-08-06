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


#include "wresall.h"
#include "autodep.h"
#include "reserr.h"
#include "wresrtns.h"

DepInfo *WResGetAutoDep( const char *fname )
{
    WResFileID      handle;
    WResDir         dir;
    int             dup_discarded;
    WResID          *name;
    WResID          *type;
    WResDirWindow   window;
    WResLangInfo    *info;
    DepInfo         *ret;
    WResFileSSize   numread;

    ret = NULL;
    handle = ResOpenFileRO( fname );
    if( handle != NIL_HANDLE ) {
        if( WResIsWResFile( handle ) && (dir = WResInitDir()) != NULL ) {
            if( !WResReadDir( handle, dir, &dup_discarded ) ) {
                name = WResIDFromStr( DEP_LIST_NAME );
                type = WResIDFromNum( DEP_LIST_TYPE );
                if( name != NULL && type != NULL ) {
                    window = WResFindResource( type, name, dir, NULL );
                    if( WResIsEmptyWindow( window ) ) {
                        WRES_ERROR( WRS_RES_NOT_FOUND );
                    } else {
                        WResIDFree( name );
                        WResIDFree( type );
                        info = WResGetLangInfo( window );
                        if( ResSeek( handle, info->Offset, SEEK_SET ) == -1 ) {
                            WRES_ERROR( WRS_SEEK_FAILED );
                        } else {
                            ret = WRESALLOC( info->Length );
                            if( ret == NULL ) {
                                WRES_ERROR( WRS_MALLOC_FAILED );
                            } else {
                                numread = WRESREAD( handle, ret, info->Length );
                                if( numread != info->Length ) {
                                    WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
                                    ret = NULL;
                                }
                            }
                        }
                    }
                }
            }
            WResFreeDir( dir );
        }
        ResCloseFile( handle );
    }
    return( ret );
}

void WResFreeAutoDep( DepInfo *ptr )
{
    WRESFREE( ptr );
}
