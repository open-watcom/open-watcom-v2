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
* Description:  handle ALL errors, warnings, and extensions
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "errcod.h"
#include "cpopt.h"
#include "progsw.h"
#include "segsw.h"
#include "stmtsw.h"
#include "global.h"
#include "cioconst.h"
#include "ferror.h"
#include "inout.h"

#include <stdarg.h>
#include <string.h>


static  void    ExtIssued( void ) {
// An extension message has just been issued.
    NumExtens++;
}

static  void    WrnIssued( void ) {
// A warning message has just been issued.
    NumWarns++;
}

static  void    ErrIssued( void ) {
// An error message has just been issued.
    if( ( ProgSw & PS_SYMTAB_PROCESS ) == 0 ) {
        CpError = TRUE;
        AError = TRUE;
    }
    NumErrors++;
    ProgSw |= PS_ERROR;
}

static  void    ErrHandler( char *err_type, int error, va_list args ) {
// Handle errors ANY time
    int         column;
    int         contline;
    byte        caret;
    bool        was_listed;
    bool        save_list;
    char        buffer[ERR_BUFF_SIZE+1];
    char        buff[MAX_SYMLEN+1];

    ChkErrFile();
    save_list = SetLst( TRUE );
    was_listed = WasStmtListed();
    caret = CarrotType( error );
    if( (SrcRecNum != 0) && // consider error opening source file
        !(ProgSw & PS_SYMTAB_PROCESS) &&
        !(ProgSw & PS_END_OF_SUBPROG) ) {
        if( StmtSw & SS_SCANNING ) {
            column = LexToken.col + 1;
            contline = LexToken.line;
        } else {
            column = 0;
            contline = 0;
            // If the message does not require a caret, then it is
            // possible to process an error when "CITNode" is NULL or
            // the "oprpos/opnpos" fields are meaningless.
            // Consider:
            // c$notime=10       CITNode == NULL
            //       end
            // or
            //       a = 2.3
            // c$notime=10       "oprpos/opnpos" fields are meaningless
            //      & + 4.2
            //       end
            if( ( caret != NO_CARROT ) && ( CITNode->link != NULL ) ) {
                if( caret == OPR_CARROT ) {
                    column = CITNode->oprpos & 0xff;
                    contline = CITNode->oprpos >> 8;
                } else {
                    column = CITNode->opnpos & 0xff;
                    contline = CITNode->opnpos >> 8;
                }
            }
        }
        if( was_listed && ( caret != NO_CARROT ) && ( column != 0 ) ) {
            memset( buffer, ' ', column + 7 );
            buffer[ column + 7 ] = '$';
            buffer[ column + 8 ] = NULLCHAR;
            PrtLstNL( buffer );
        }
    }
    if( CurrFile != NULL ) {
        if( ( SrcRecNum == 0 ) ||
            ( ProgSw & PS_SYMTAB_PROCESS ) ||
            ( ProgSw & PS_END_OF_SUBPROG ) ) {
            MsgFormat( "%s1: ", buffer, CurrFile->name );
        } else {
            MsgFormat( "%s1(%d2): ", buffer, CurrFile->name,
                       SrcRecNum + contline );
        }
        JustErr( buffer );
    }
    PrintErr( err_type );
    BldErrCode( error, buffer );
    PrintErr( buffer );
    if( SrcRecNum != 0 ) {
        if( (ProgSw & PS_SYMTAB_PROCESS) || (ProgSw & PS_END_OF_SUBPROG) ) {
            // We may not know the subprogram name.
            // Consider:
            //        SUBROUTINE ()
            if( SubProgId != NULL ) {
                STGetName( SubProgId, buff );
                if( was_listed ) {
                    MsgJustErr( MS_IN, buff );
                } else {
                    MsgPrintErr( MS_IN, buff );
                }
            }
        } else if( caret != NO_CARROT ) {
            if( column == 0 ) {
                // regardless of whether statement was listed or not we want
                // to display "at end of statement"
                MsgPrintErr( MS_AT_EOSTMT );
            } else {
                if( was_listed ) {
                    MsgJustErr( MS_COLUMN, column );
                } else {
                    MsgPrintErr( MS_COLUMN, column );
                }
            }
        }
    }
    __BldErrMsg( error, buffer, args );
    PrintErr( buffer );
    PrtErrNL();
    SetLst( save_list );
}

void    Error( int code, ... ) {
// Error message handler
    va_list     args;

    va_start( args, code );
    ErrHandler( "*ERR*", code, args );
    va_end( args );
    ErrIssued();
}

void    Warning( int code, ... ) {
// Warning message handler
    va_list     args;

    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) return;
    if( ( Options & OPT_WARN ) != 0 ) {
        va_start( args, code );
        ErrHandler( "*WRN*", code, args );
        va_end( args );
    }
    WrnIssued();
}

void    Extension( int code, ... ) {
// Extension Message Handler
    va_list     args;

    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) return;
    if( ( Options & OPT_EXT ) != 0 ) {
        va_start( args, code );
        ErrHandler( "*EXT*", code, args );
        va_end( args );
    }
    ExtIssued();
}

void    InfoError( int code, ... ) {
// Informational error - should not affect compilation.
    va_list     args;

    NumErrors++;
    ProgSw |= PS_ERROR;
    va_start( args, code );
    ErrHandler( "*ERR*", code, args );
    va_end( args );
}
