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


#include <string.h>

#include "plusplus.h"
#include "preproc.h"
#include "name.h"
#include "memmgr.h"
#include "errdefns.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "ptree.h"
#include "scan.h"
#include "asmstmt.h"

#if _CPU == 8086 || _CPU == 80386
#include "asmsym.h"
#endif

#ifdef DISABLE_ASM_STMT

PTREE AsmStmt( void )
{
    return NULL;
}

#else

static void ensureBufferReflectsCurToken( void )
{
    if( TokenUsesBuffer( CurToken ) ) {
        if( CurToken == T_CONSTANT ) {
            // kludge to handle the fact that rewrites don't store
            // the text for integral constants
            switch( ConstType ) {
            case TYP_FLOAT:
            case TYP_DOUBLE:
            case TYP_LONG_DOUBLE:
                // OK, Buffer is set
                break;
            case TYP_UCHAR:
            case TYP_UINT:
            case TYP_ULONG:
                ultoa( U32Fetch( Constant64 ), Buffer, 10 );
                break;
            default:
                ltoa( U32Fetch( Constant64 ), Buffer, 10 );
            }
        }
    } else {
        strcpy( Buffer, Tokens[ CurToken ] );
    }
}

static PTREE genFnCall( char *name )
{
    return( PTreeBinary( CO_CALL, PTreeId( name ), NULL ) );
}

static boolean endOfAsmStmt( void )
{
    if( CurToken == T_EOF ) return( TRUE );
    if( CurToken == T_NULL ) return( TRUE );
    if( CurToken == T___ASM ) return( TRUE );
    if( CurToken == T_RIGHT_BRACE ) return( TRUE );
    if( CurToken == T_SEMI_COLON ) return( TRUE );
    return( FALSE );
}

static void absorbASMConstant( char *buff, unsigned size )
{
    // 0a0b3h is a valid .ASM constant
    for(;;) {
        NextToken();
        if( endOfAsmStmt() ) {
            return;
        }
        ensureBufferReflectsCurToken();
        if(( CharSet[ Buffer[0] ] & (C_AL|C_DI) ) == 0 ) {
            return;
        }
        strncat( buff, Buffer, size );
    }
}

static boolean isId( unsigned token )
{
    if( token == T_ID ) {
        return( TRUE );
    }
    if( token >= FIRST_KEYWORD && token <= LAST_KEYWORD ) {
        return( TRUE );
    }
    return( FALSE );
}

static void getAsmLine( VBUF *buff )
{
    char line[256];

    /* reserve at least MAX_INSTR_SIZE bytes in the buffer */
    VbufReqd( buff, (( AsmSysAddress()+MAX_INSTR_SIZE) + (MAX_INSTR_SIZE-1) ) & ~(MAX_INSTR_SIZE-1) );
    AsmSysSetCodeBuffer( buff->buf );
    ensureBufferReflectsCurToken();
    if( isId( CurToken ) && strcmp( Buffer, "__emit" ) == 0 ) {
        strcpy( line, AsmSysDefineByte() );
        strcat( line, " " );
        NextToken();
        ensureBufferReflectsCurToken();
    } else {
        line[0] = '\0';
    }
    for(;;) {
        if( endOfAsmStmt() ) break;
        strncat( line, Buffer, sizeof(line)-1 );
        if( CurToken == T_CONSTANT ) {
            absorbASMConstant( line, sizeof(line)-1 );
            strncat( line, " ", sizeof(line)-1 );
        } else {
            if( isId( CurToken ) ) {
                NextToken();
                if( CurToken != T_XOR ) {
                    strncat( line, " ", sizeof(line)-1 );
                }
            } else {
                NextToken();
            }
            ensureBufferReflectsCurToken();
        }
    }
    if( line[0] != '\0' ) {
        AsmSysParseLine( line );
    }
    VbufUsed( buff, AsmSysAddress() );
    if( CurToken == T_SEMI_COLON ) {
        // ; .ASM comment
        for(;;) {
            NextToken();
            if( CurToken == T_EOF ) break;
            if( CurToken == T_NULL ) break;
        }
    }
}

static void advancePastT_NULL( void )
{
    // won't advance past EOL unless we do this
    PPState = PPS_NORMAL;
    NextToken();
    PPState = PPS_EOL;
}

PTREE AsmStmt( void )
/*******************/
{
    boolean uses_auto;
    void *aux_info;
    unsigned skip_token;
    PTREE expr;
    TYPE fn_type;
    TYPE ret_type;
    SYMBOL sym;
    char *fn_name;
    auto VBUF code_buffer;

    PPState = PPS_EOL;
    VbufInit( &code_buffer );
    NextToken();
    while( CurToken == T_NULL ) {
        advancePastT_NULL();
    }
    AsmSysInit();
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        for(;;) {
            getAsmLine( &code_buffer );
            if( CurToken == T_RIGHT_BRACE ) break;
            if( CurToken == T_EOF ) break;
            if( CurToken == T_NULL ) {
                advancePastT_NULL();
            } else {
                NextToken();
            }
        }
        skip_token = T_RIGHT_BRACE;
    } else {
        getAsmLine( &code_buffer );
        skip_token = T_NULL;
    }
    PPState = PPS_NORMAL;
    if( CurToken == skip_token ) {
        NextToken();
    }
    if( AsmSysAddress() != 0 ) {
        fn_name = NameDummy();
        aux_info = AsmSysCreateAux( fn_name );
        uses_auto = AsmSysInsertFixups( &code_buffer );
        if( uses_auto ) {
            AsmSysUsesAuto();
        }
        AsmSysDone();
        ret_type = GetBasicType( TYP_VOID );
        fn_type = MakeModifiableFunction( ret_type, NULL );
        fn_type->u.f.pragma = aux_info;
        fn_type = CheckDupType( fn_type );
        sym = SymCreateFileScope( fn_type, SC_NULL, SF_NULL, fn_name );
        LinkageSet( sym, "C" );
        expr = genFnCall( fn_name );
    } else {
        expr = NULL;
    }
    VbufFree( &code_buffer );
    return( expr );
}

#endif
