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
* Description:  Inline assembler statement processing.
*
****************************************************************************/


#include "cvars.h"
#include "asmstmt.h"
#include "pragdefn.h"

#define MAX_ASM_LINE_LEN 511

static int EndOfAsmStmt( void )
/*****************************/
{
    if( CurToken == T_SEMI_COLON ) {
        // ; .ASM comment
        for( ;; ) {
            NextToken();
            if( ( CurToken == T_EOF )
              || ( CurToken == T_NULL ) ) {
                break;
            }
        }
        return( TRUE );
    }
    if( CurToken == T_EOF )
        return( TRUE );
    if( CurToken == T_NULL )
        return( TRUE );
    if( CurToken == T___ASM )
        return( TRUE );
    if( CurToken == T_RIGHT_BRACE )
        return( TRUE );
    return( FALSE );
}

static void GetAsmLine( void )
/****************************/
{
    unsigned    AsmErrLine;
    TOKEN       LastToken;
    char        buf[ MAX_ASM_LINE_LEN + 1 ];

    CompFlags.pre_processing = 1;
    AsmErrLine = TokenLoc.line;
    *buf = '\0';
    if( strcmp( Buffer, "_emit" ) == 0 ) {
        strcpy( buf, AsmSysDefineByte() );
        NextToken();
    }
    LastToken = T_DOT;
    for( ;; ) {
        if( EndOfAsmStmt() )
            break;
        if(( LastToken != T_DOT )
          && ( LastToken != T_BAD_CHAR )
          && ( CurToken != T_XOR ))
            strncat( buf, " ", MAX_ASM_LINE_LEN );
        strncat( buf, Buffer, MAX_ASM_LINE_LEN );
        LastToken = CurToken;
        NextToken();
    }
    buf[ MAX_ASM_LINE_LEN ] = '\0';
    if( *buf != '\0' ) {
        TokenLoc.line = AsmErrLine;
        AsmSysLine( buf );
    }
    CompFlags.pre_processing = 0;
}

void AsmStmt( void )
/******************/
{
    int             too_many_bytes;
    unsigned char   buff[ MAXIMUM_BYTESEQ + 32 ];
    TOKEN           skip_token;

    // indicate that we are inside an __asm statement so scanner will
    // allow tokens unique to the assembler. e.g. 21h
    CompFlags.inside_asm_stmt = 1;

    NextToken();
    AsmSysInit( buff );
    too_many_bytes = 0;
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        for( ;; ) {             // grab assembler lines
            GetAsmLine();
            if( AsmCodeAddress > MAXIMUM_BYTESEQ ) {
                if( ! too_many_bytes ) {
                    CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                    too_many_bytes = 1;
                }
                // reset index to we don't overrun buffer
                AsmCodeAddress = 0;
            }
            if( CurToken == T_RIGHT_BRACE )
                break;
            if( CurToken == T_EOF )
                break;
            NextToken();
        }
        skip_token = T_RIGHT_BRACE;
    } else {
        GetAsmLine();           // grab single assembler instruction
        skip_token = T_NULL;
    }
    CompFlags.pre_processing = 0;
    CompFlags.inside_asm_stmt = 0;
    AsmSysMakeInlineAsmFunc( too_many_bytes );
    AsmSysFini();
    if( CurToken == skip_token ) {
        NextToken();
    }
}
