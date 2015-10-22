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
* Description:  Error message output.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbginvk.h"
#include "dbglit.h"
#include "dip.h"
#include "dbgerr.h"
#include "dui.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgstk.h"
#include "dbgexpr.h"
#include "dbgcall2.h"
#include "dbgbrk.h"
#include "dbgpend.h"
#include "dbgprog.h"


extern int              ScanSavePtr;

extern void             CmdError( void );
extern void             CaptureError( void );
extern void             PurgeSymHandles( void );
extern void             DlgCmd( void );
extern void             StartupErr( const char * );
extern void             Suicide( void );

/*
 * Error - output error message and suicide
 */

void Error( dbg_err_flags flg, char *fmt, ... )
{
    char            buff[TXT_LEN];
    va_list         args;
    char            *ptr;
    invokes         *inv;
    input_stack     *inp;
    char            *where;
    bool            cmderror;

    va_start( args, fmt );
    ptr = buff;
    if( flg & ERR_INTERNAL ) {
        ptr = StrCopy( LIT_ENG( Internal_Error ), ptr );
    }
    ptr = FmtStr( ptr, fmt, args );
    ptr = StrCopy( ".", ptr );
    if( flg & ERR_LOC ) {
        ptr = StrCopy( "\n    ", ptr );
        switch( CurrToken ) {
        case T_CMD_SEPARATOR:
            ptr = StrCopy( LIT_ENG( ERR_NEAR_END_OF_COMMAND ), ptr );
            break;
        case T_LINE_SEPARATOR:
            ptr = StrCopy( LIT_ENG( ERR_NEAR_END_OF_LINE ), ptr );
            break;
        default:
            if( ScanLen() == 0 ) Scan();
            ptr = Format( ptr, LIT_ENG( ERR_NEAR_TOKEN ), ScanPos(), ScanLen() );
            break;
        }
    }
    SetProgStartHook( TRUE );
    UnFreezeRegs();
    ScanExpr( NULL );
    ExprPurge();
    PurgeSymHandles(); /* must be done after ExprPurge */
    DIPCancel();
    ScanSavePtr = 0; /* clean up previous ScanSave locations */

    if( _IsOff( SW_ERROR_PRESERVES_RADIX ) ) {
        RestoreRadix();
    }
    _SwitchOff( SW_CALL_FATAL );
    if( _IsOn( SW_ERROR_STARTUP ) ) {
        StartupErr( buff );
    }
    if( _IsOn( SW_ERR_IN_TXTBUFF ) ) {
        PurgeInpStack();
        StrCopy( buff, TxtBuff );
        DUIArrowCursor();
        Suicide();
    }
    if( (flg & ERR_SILENT) == 0 ) {
        where = LIT_ENG( ERR_EXECUTING_AT );
        for( inp = InpStack; inp != NULL; inp = inp->link ) {
            if( inp->type & INP_CMD_FILE ) {
                inv = inp->handle;
                ptr = StrCopy( "\n    ", ptr );
                ptr = Format( ptr, where, inv->line, inv->name );
                where = LIT_ENG( ERR_CALLED_FROM );
            }
        }
        DUIFlushKeys();
        DUIWndDebug();
        RingBell();
        DUIErrorBox( buff );
    }
    cmderror = FALSE;
    for( inp = InpStack; inp != NULL; inp = inp->link ) {
        if( inp->type & INP_BREAK_POINT ) {
            BrkCmdError();
        }
        if( inp->type & INP_CAPTURED ) {
            CaptureError();
        }
        if( inp->type & INP_DLG_CMD ) {
            cmderror = TRUE;
        }
    }
    PurgeInpStack();
    if( cmderror && fmt != LIT_ENG( ERR_DBG_INTERRUPT ) ) {
        DlgCmd();
        ProcInput();
    }
    if( _IsOn( SW_ERROR_RETURNS ) ) return;
    DUIArrowCursor();
    Suicide();
}

void PrevError( const char *msg )
{
    DUIWndDebug();
    RingBell();
    DUIErrorBox( msg );
}
