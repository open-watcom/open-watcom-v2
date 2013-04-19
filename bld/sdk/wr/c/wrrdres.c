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


#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrrdres.h"
#include "wrmsg.h"
#include "wrstrdup.h"
#include "wrinfoi.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

static int loadResDirFromRES( WRInfo *info, char *filename, int *is_wres )
{
    WResFileID  file_handle;
    int         dup_discarded;
    int         ok;

    ok = ((file_handle = ResOpenFileRO( filename )) != -1);

    if( ok ) {
        *is_wres = WResIsWResFile( file_handle );
    }

    if( ok ) {
        ok = ((info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        ok = !WResReadDir( file_handle, info->dir, &dup_discarded );
        if( ok && dup_discarded ) {
            WRDisplayErrorMsg( WR_DUPRESDISCARD );
        }
    }

    if( file_handle != -1 ) {
        ResCloseFile( file_handle );
    }

    return( ok );
}

int WRLoadResDirFromRES( WRInfo *info, int *is_wres )
{
    return( loadResDirFromRES( info, info->file_name, is_wres ) );
}

int WRLoadResourceFromRES( WRInfo *info )
{
    WResTargetOS        target_os;
    WRFileType          target;
    int                 is_wres;
    int                 ok;

    ok = WRLoadResDirFromRES( info, &is_wres );

    if( ok ) {
        target_os = WResGetTargetOS( info->dir );
        target = WR_INVALID_FILE;
        switch( target_os ) {
        case WRES_OS_WIN16:
            target = WR_WIN16M_RES;
            if( is_wres ) {
                target = WR_WIN16W_RES;
            }
            break;
        case WRES_OS_WIN32:
            target = WR_WINNTM_RES;
            if( is_wres ) {
                target = WR_WINNTW_RES;
            }
            break;
        }
        if( target == WR_INVALID_FILE ) {
            WRDisplayErrorMsg( WR_INVALIDFILE );
            ok = FALSE;
        } else if( target != info->file_type ) {
            if( target == WR_WIN16W_RES ) {
                WRDisplayErrorMsg( WR_BADFILEWWIN16 );
            } else if( target == WR_WINNTW_RES ) {
                WRDisplayErrorMsg( WR_BADFILEWWINNT );
            } else if( target == WR_WIN16M_RES ) {
                WRDisplayErrorMsg( WR_BADFILEMWIN16 );
            } else if( target == WR_WINNTM_RES ) {
                WRDisplayErrorMsg( WR_BADFILEMWINNT );
            }
            ok = FALSE;
        }
    }

    return( ok );
}

int WRLoadResourceFrom_RC( WRInfo *info )
{
    WResTargetOS        target_os;
    WRFileType          target;
    char                fn_path[_MAX_PATH];
    int                 is_wres;
    int                 ok;

    ok = (info != NULL && info->file_name != NULL);

    if( ok ) {
        WRGetInternalRESName( info->file_name, fn_path );
    }

#ifndef __NT__
    if( ok ) {
        target = WRIdentifyFile( fn_path );
        ok = !WRIs32Bit( target );
        if( !ok ) {
            WRDisplayErrorMsg( WR_NOLOAD32IN16 );
        }
    }
#endif

    if( ok ) {
        ok = loadResDirFromRES( info, fn_path, &is_wres );
    }

    if( ok ) {
        target_os = WResGetTargetOS( info->dir );
        target = WR_INVALID_FILE;
        switch( target_os ) {
        case WRES_OS_WIN16:
            target = WR_WIN16M_RES;
            if( is_wres ) {
                target = WR_WIN16W_RES;
            }
            break;
        case WRES_OS_WIN32:
            target = WR_WINNTM_RES;
            if( is_wres ) {
                target = WR_WINNTW_RES;
            }
            break;
        }
        info->internal_type = target;
        if( target == WR_INVALID_FILE ) {
            WRDisplayErrorMsg( WR_INVALIDFILE );
            ok = FALSE;
        }
    }

    if( ok ) {
        info->internal_filename = WRStrDup( fn_path );
        ok = (info->internal_filename != NULL);
    }

    return( ok );
}
