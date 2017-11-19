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
    WResFileID      fid;
    WResDir         dir;
    bool            dup_discarded;
    WResID          *name;
    WResID          *type;
    WResDirWindow   window;
    WResLangInfo    *info;
    DepInfo         *ret;
    size_t          numread;

    ret = NULL;
    fid = ResOpenFileRO( fname );
    if( fid != NULL ) {
        if( WResIsWResFile( fid ) && (dir = WResInitDir()) != NULL ) {
            if( !WResReadDir( fid, dir, &dup_discarded ) ) {
                name = WResIDFromStr( DEP_LIST_NAME );
                type = WResIDFromNum( DEP_LIST_TYPE );
                if( name != NULL && type != NULL ) {
                    window = WResFindResource( type, name, dir, NULL );
                    if( WResIsEmptyWindow( window ) ) {
                        WRES_ERROR( WRS_RES_NOT_FOUND );
                    } else {
                        info = WResGetLangInfo( window );
                        if( WRESSEEK( fid, info->Offset, SEEK_SET ) ) {
                            WRES_ERROR( WRS_SEEK_FAILED );
                        } else {
                            ret = WRESALLOC( info->Length );
                            if( ret == NULL ) {
                                WRES_ERROR( WRS_MALLOC_FAILED );
                            } else {
                                if( (numread = WRESREAD( fid, ret, info->Length )) != (size_t)info->Length ) {
                                    WRES_ERROR( WRESIOERR( fid, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
                                    ret = NULL;
                                }
                            }
                        }
                    }
                }
                if( name != NULL ) {
                    WResIDFree( name );
                }
                if( type != NULL ) {
                    WResIDFree( type );
                }
            }
            WResFreeDir( dir );
        }
        ResCloseFile( fid );
    }
    return( ret );
}

void WResFreeAutoDep( DepInfo *ptr )
{
    WRESFREE( ptr );
}
