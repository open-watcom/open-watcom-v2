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
* Description:  Run a debugger command.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "dbgio.h"
#include "spawn.h"
#include "dui.h"


extern char             *ReScan( char * );
extern void             PushInpStack( void *, bool (*rtn)( void *, inp_rtn_action ), bool );
extern void             TypeInpStack( input_type );
extern bool             TBreak( void );

static bool DoneCmd( void *buff, inp_rtn_action action )
{
    switch( action ) {
    case INP_RTN_INIT:
        ReScan( buff );
        return( TRUE );
    case INP_RTN_EOL:
        return( FALSE );
    case INP_RTN_FINI:
        DbgFree( buff );
        return( TRUE );
    }
    return( FALSE ); // silence compiler
}


static void DoOneCmd( void *_cmd )
{
    char        *cmd = _cmd;

    if( cmd[0] == NULLCHAR ) {
        DUIDlgTxt( LIT( Empty ) );
    } else {
        PushInpStack( cmd, DoneCmd, FALSE );
        TypeInpStack( INP_DLG_CMD );
        TBreak();   /* clear any pending terminal interrupts */
    }
}

void DoCmd( char *cmd )
{
    SpawnP( DoOneCmd, cmd );
}
