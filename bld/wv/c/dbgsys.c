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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dui.h"
#include "dbgio.h"
#include "dbgscan.h"


extern long         _fork( const char *, size_t );
extern rc_erridx    RemoteFork( const char *,size_t );
extern void         RemoteSuspend(void);
extern void         RemoteResume(void);
extern bool         CheckPointMem( unsigned, char * );
extern void         CheckPointRestore( void );
extern char         *GetCmdName( int );

static const char SystemOps[] = { "Remote\0Local\0" };

void DoSystem( const char *cmd, size_t len, int loc )
{
    long        rc;
    rc_erridx   ret;
    bool        chk;

    DUISysStart();
    if( loc == 0 && _IsOn( SW_REMOTE_FILES ) )
        loc = 1;
    if( loc > 0 ) {
        ret = RemoteFork( cmd, len );
        rc = 0;
    } else {
        RemoteSuspend();
        chk = CheckPointMem( CheckSize, TxtBuff );
        rc = _fork( cmd, len );
        if( chk )
            CheckPointRestore();
        RemoteResume();
    }
    DUISysEnd( rc >= 0 );
    if( rc < 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_SYS_FAIL ), StashErrCode( rc & 0xFFFF, OP_LOCAL ) );
    }
}


void ProcSystem( void )
{
    const char  *start;
    size_t      len;
    int         loc;

    loc = 0;
    if( CurrToken == T_DIV ) {
        Scan();
        loc = ScanCmd( SystemOps );
        if( loc == 0 ) {
            Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), GetCmdName( CMD_SYSTEM ) );
        }
        if( loc == 2 ) loc = -1;
    }
    ScanItem( FALSE, &start, &len );
    ReqEOC();
    DoSystem( start, len, loc );
}
