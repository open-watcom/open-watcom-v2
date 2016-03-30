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
* Description:  Compile time source input, listing output.
*
****************************************************************************/


#include "ftnstd.h"
#include "progsw.h"
#include "extnsw.h"
#include "errcod.h"
#include "cpopt.h"
#include "global.h"
#include "intcnv.h"
#include "extnsw.h"
#include "csetinfo.h"
#include "ferror.h"
#include "comio.h"
#include "inout.h"
#include "charset.h"
#include "fmacros.h"
#include "option.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


extern  char    *SkipBlanks( char * );

static void Comment( void )
{
// Process a comment for possible compiler directives.

    int old_srcrecnum;

    if( ( SrcBuff[ 0 ] != NULLCHAR ) && ( SrcBuff[ 1 ] == '$' ) ) {
        old_srcrecnum = SrcRecNum;
        SrcRecNum = CurrFile->rec; // in case we get an error processing comment
        SrcOption();
        SrcRecNum = old_srcrecnum;
    } else {
        ComPrint();
    }
}

void ComRead( void )
{
    char        *cursor;
    uint        column;
    char        ch;
    byte        chtype;
    uint        stmt_type;
    unsigned_32 stmt_no;
    bool        stno_found;
    byte        cont_type;
    bool        done_scan;
    ftnoption   save_options;

    // Comment processor sets "Options" so that
    //          c$warn
    //          c$notime=5
    // is diagnosed if /nowarn is specified
    // "Options" must be saved so that we don't get an unreferenced warning
    // message in the following case.
    //          c$noreference
    //                  integer*4 sam
    //          c$reference
    //                  end
    save_options = Options;
    stmt_type = STMT_COMMENT;
    cont_type = 0;
    cursor = 0;
    column = FIRST_COL - 1;
    stmt_no = 0;
    stno_found = FALSE;
    done_scan = FALSE;
    for(;;) {
        ReadSrc();
        if( ProgSw & PS_SOURCE_EOF ) break;
        if( CurrFile->flags & CONC_PENDING ) break;
        // column starts off before current column
        column = FIRST_COL - 1;
        cursor = SrcBuff;
        ch = *cursor;
        if( ( ch != 'C' ) && ( ch != 'c' ) && ( ch != '*' ) ) {
            if( ProgSw & PS_SKIP_SOURCE ) continue;
            if( ( ch == 'D' ) || ( ch == 'd' ) ) {
                if( !(ExtnSw & XS_D_IN_COLUMN_1) ) {
                    Extension( CC_D_IN_COLUMN_1 );
                    ExtnSw |= XS_D_IN_COLUMN_1;
                }
                if( !CompileDebugStmts() ) continue;
                ch = ' ';
            }
            // not a comment (but it might be a blank line)
            // try for a statement number
            stmt_no = 0;
            stno_found = FALSE;
            for(;;) {
                chtype = CharSetInfo.character_set[ (unsigned char)ch ];
                if( chtype == C_EL ) break;
                if( ( chtype == C_CM ) && ( column != CONT_COL - 1 ) ) {
                    if( ( ExtnSw & XS_EOL_COMMENT ) == 0 ) {
                        Extension( CC_EOL_COMMENT );
                        ExtnSw |= XS_EOL_COMMENT;
                    }
                    break;
                }
                if( chtype == C_SP ) {
                    ++column;
                } else if( chtype == C_TC ) {
                    column += 8 - column % 8;
                } else if( CharSetInfo.is_double_byte_blank( cursor ) ) {
                    cursor++;
                    column += 2;
                } else {
                    // a digit in the statement number field
                    ++column;
                    if( ( chtype == C_DI ) && ( column != CONT_COL ) ) {
                        stmt_type = STMT_START;
                        if( column > CONT_COL ) {
                            done_scan = TRUE;
                            break;
                        }
                        stmt_no = 10 * stmt_no + ch - '0';
                        stno_found = TRUE;
                    } else {
                        stmt_type = STMT_START;
                        if( column != CONT_COL ) {
                            done_scan = TRUE;
                            break;
                        }
                        // its in the continuation column
                        if( ch != '0' ) {
                            // we have a genuine continuation line
                            // but save the type for later diagnosis
                            cont_type = chtype;
                            stmt_type = STMT_CONT;
                            // position to column 7
                            ++column;
                            ++cursor;
                            done_scan = TRUE;
                            break;
                        }
                    }
                }
                ++cursor;
                ch = *cursor;
                if( column >= LastColumn ) {
                    // Consider:                             Column 73
                    //     1                                     |
                    //     0123                                  2001
                    *cursor = NULLCHAR;
                    break;
                }
            }
            if( done_scan ) break;
            if( stmt_type != STMT_COMMENT ) break;
        }
        Comment();
        // quit if the comment simulates EOF (i.e. C$DATA)
        if( ProgSw & PS_SOURCE_EOF ) break;
        // quit if c$include encountered
        if( CurrFile->flags & INC_PENDING ) break;
    }
    Cursor = cursor;
    Column = column - 1;
    NextStmtNo = stmt_no;
    StmtNoFound = stno_found;
    StmtType = stmt_type;
    ContType = cont_type;
    Options = save_options;
}


void ProcInclude( void )
{
    int old_srcrecnum;

    ComPrint();
    if( strlen( SrcBuff ) > LastColumn ) {
        SrcBuff[ LastColumn ] = NULLCHAR;
    }
    old_srcrecnum = SrcRecNum;
    SrcRecNum = CurrFile->rec; // in case we get an error processing INCLUDE
    Include( SkipBlanks( &SrcBuff[ 10 ] ) );
    SrcRecNum = old_srcrecnum;
}

static void PrintLineInfo( char *buffer )
{
    PrtLst( buffer );
    if( CurrFile->link == NULL ) {
        PrtLst( " " );
    } else {
        PrtLst( "+" );
    }
    PrtLstNL( SrcBuff );
}


void FmtInteger( char *buff, int num, int width )
{
    sprintf( buff, "%*d", width, num );
}

void ComPrint( void )
{
    char        buffer[8];

    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) return;
    FmtInteger( buffer, CurrFile->rec, 7 );
    PrintLineInfo( buffer );
}

void LinePrint( void )
{
    char        buffer[8];

    ISNNumber++;
    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) return;
    FmtInteger( buffer, CurrFile->rec, 7 );
    PrintLineInfo( buffer );
}


