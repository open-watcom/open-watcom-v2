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


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dui.h"


extern long         _fork( char *, unsigned int );
extern long         RemoteFork(char *,unsigned int );
extern void         RemoteSuspend(void);
extern void         RemoteResume(void);
extern unsigned int ScanCmd(char *);
extern void         ReqEOC(void);
extern void         Scan(void);
extern bool         ScanItem(bool ,char **,unsigned int *);
extern bool         CheckPointMem( unsigned, char * );
extern void         CheckPointRestore( void );
extern char         *GetCmdName( int );

extern tokens       CurrToken;
extern unsigned     CheckSize;
extern char         *TxtBuff;

static char SystemOps[] = { "Remote\0Local\0" };

void ProcSystem()
{
    char        *start;
    unsigned    len;
    int         loc;

    loc = 0;
    if( CurrToken == T_DIV ) {
        Scan();
        loc = ScanCmd( SystemOps );
        if( loc == 0 ) {
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_SYSTEM ) );
        }
        if( loc == 2 ) loc = -1;
    }
    ScanItem( FALSE, &start, &len );
    ReqEOC();
    DoSystem( start, len, loc );
}

void DoSystem( char *cmd, unsigned len, int loc )
{
    long        ret;
    bool        chk;

    DUISysStart();
    if( loc == 0 && _IsOn( SW_REMOTE_FILES ) ) loc = 1;
    if( loc > 0 ) {
        ret = RemoteFork( cmd, len );
    } else {
        RemoteSuspend();
        chk = CheckPointMem( CheckSize, TxtBuff );
        ret = _fork( cmd, len );
        if( chk ) CheckPointRestore();
        RemoteResume();
    }
    DUISysEnd( ret >= 0 );
    if( ret < 0 ) Error( ERR_NONE, LIT( ERR_SYS_FAIL ), (int) ret );
}
