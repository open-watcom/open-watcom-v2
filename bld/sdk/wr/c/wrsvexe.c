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


#include <wwindows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "watcom.h"
#include "global.h"    /* this is a WRC header file */
#include "rcio.h"      /* this is a WRC header file */
#include "pass2.h"     /* this is a WRC header file */
#include "rcspawn.h"   /* this is a WRC header file */

#include "wresset2.h"

#include "wrglbl.h"
#include "wrsvres.h"
#include "wrmsg.h"
#include "wrsvexe.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static int  WRExecRCPass2( void );
static int  WRPass2( void );
static int  WRSaveResourceToEXE( WRInfo *, int, WRFileType );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

jmp_buf     jmpbuf_RCFatalError;

/* this function duplicates Pass2 in rc.c of the WRC project */
int WRPass2( void )
{
    int ok;

    ok = RcPass2IoInit();

    if( ok ) {
        switch( Pass2Info.OldFile.Type ) {
        case EXE_TYPE_NE_WIN:
            ok = MergeResExeNE();
            break;
        case EXE_TYPE_PE:
            ok = MergeResExePE();
            break;
        }

        RcPass2IoShutdown( ok );
    }

    return( ok );
}

int WRExecRCPass2( void )
{
    int ret;
    int ok;

    ret = setjmp( jmpbuf_RCFatalError );

    if( ret ) {
        ok = FALSE;
    } else {
        ok = WRPass2();
    }

    return( ok );
}

int WRSaveResourceToWin16EXE( WRInfo *info, int backup )
{
    int ok;

    ok = (info->file_type != WR_WINNT_EXE && info->file_type != WR_WINNT_DLL);

    if( !ok ) {
        WRDisplayErrorMsg( WR_NOCONV1632 );
    }

    if( ok ) {
        ok = (info->file_type == WR_WIN16_EXE || info->file_type == WR_WIN16_DLL);
        if( !ok ) {
            WRDisplayErrorMsg( WR_NOTWIN16 );
        }
    }

    if( ok ) {
        ok = WRSaveResourceToEXE( info, backup, WR_WIN16W_RES );
    }

    return( ok );
}

int WRSaveResourceToWinNTEXE( WRInfo *info, int backup )
{
    int ok;

    ok = (info->file_type != WR_WIN16_EXE && info->file_type != WR_WIN16_DLL);

    if( !ok ) {
        WRDisplayErrorMsg( WR_NOCONV1632 );
    }

    if( ok ) {
        ok = (info->file_type == WR_WINNT_EXE || info->file_type == WR_WINNT_DLL);
        if( !ok ) {
            WRDisplayErrorMsg( WR_NOTWINNT );
        }
    }

    if( ok ) {
        ok = WRSaveResourceToEXE( info, backup, WR_WINNTW_RES );
    }

    return( ok );
}

int WRSaveResourceToEXE( WRInfo *info, int backup, WRFileType ttype )
{
    int         ok;
    char        *tmp_res;
    char        *sname;
    WRFileType  stype;

    sname = NULL;

    ok = (info->file_name != NULL);

    ok = (ok && (tmp_res = WRGetTempFileName( "res" )) != NULL);

    if( ok ) {
        sname = info->save_name;
        stype = info->save_type;
        info->save_name = tmp_res;
        info->save_type = ttype;
        ok = WRSaveResource( info, FALSE );
    }

    if( ok ) {
        if( backup && WRFileExists( sname ) ) {
            ok = WRBackupFile( sname, FALSE );
        }
    }

    if( ok ) {
        memset( &CmdLineParms, 0, sizeof( CmdLineParms ) );
        CmdLineParms.Pass2Only = TRUE;
        CmdLineParms.VersionStamp = VERSION_31_STAMP;
        if( ttype == WR_WINNTW_RES ) {
            CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
        } else {
            CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
        }
        strcpy( CmdLineParms.InFileName, tmp_res );
        strcpy( CmdLineParms.InExeFileName, info->file_name );
        if( stricmp( sname, info->file_name ) ) {
            strcpy( CmdLineParms.OutExeFileName, sname );
        } else {
            strcpy( CmdLineParms.OutExeFileName, info->file_name );
        }
        ok = WRExecRCPass2();
    }

    if( tmp_res != NULL ) {
        WRDeleteFile( tmp_res );
        MemFree( tmp_res );
    }

    if( sname != NULL ) {
        info->save_name = sname;
        info->save_type = stype;
    }

    return( ok );
}

void WRInitRcGlobal( HINSTANCE inst )
{
    inst = inst;
}
