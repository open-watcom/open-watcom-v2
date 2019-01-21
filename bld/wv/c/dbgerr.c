/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include "dbglkup.h"
#include "dbginit.h"
#include "dlgcmd.h"
#include "dbgcapt.h"
#include "wspawn.h"


extern int              ScanSavePtr;

extern void             CmdError( void );

/*
 * error - output error message
 */

static int error( dbg_err_flags flg, char *fmt, va_list args )
{
    char            buff[TXT_LEN];
    char            *ptr;
    invokes         *inv;
    input_stack     *inp;
    char            *where;
    bool            cmderror;

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
            if( ScanLen() == 0 )
                Scan();
            ptr = Format( ptr, LIT_ENG( ERR_NEAR_TOKEN ), ScanPos(), ScanLen() );
            break;
        }
    }
    SetProgStartHook( true );
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
    } else {
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
            PrevError( buff );
        }
        cmderror = false;
        for( inp = InpStack; inp != NULL; inp = inp->link ) {
            if( inp->type & INP_BREAK_POINT ) {
                BrkCmdError();
            }
            if( inp->type & INP_CAPTURED ) {
                CaptureError();
            }
            if( inp->type & INP_DLG_CMD ) {
                cmderror = true;
            }
        }
        PurgeInpStack();
        if( cmderror && fmt != LIT_ENG( ERR_DBG_INTERRUPT ) ) {
            DlgCmd();
            ProcInput();
        }
        if( _IsOn( SW_ERROR_RETURNS ) ) {
            return( 0 );
        }
    }
    return( 1 );
}

/*
 * Error - output error message and suicide
 */

void Error( dbg_err_flags flg, char *fmt, ... )
/* this function never return to the caller */
{
    va_list args;

    va_start( args, fmt );
    error( flg, fmt, args );
    va_end( args );
    DUIArrowCursor();
    Suicide();
}

/*
 * ErrorRet - output error message
 */

void ErrorRet( dbg_err_flags flg, char *fmt, ... )
/* this function return to the caller */
{
    va_list args;
    int     rc;

    _SwitchOn( SW_ERROR_RETURNS );
    va_start( args, fmt );
    rc = error( flg, fmt, args );
    va_end( args );
    _SwitchOff( SW_ERROR_RETURNS );
    if( rc ) {
        DUIArrowCursor();
        Suicide();
    }
}

void PrevError( const char *msg )
/*******************************/
{
    DUIWndDebug();
    RingBell();
    DUIErrorBox( msg );
}

void StartupErr( const char *err )
/********************************/
{
    PopErrBox( err );
    KillDebugger(1);
    // never return
}
