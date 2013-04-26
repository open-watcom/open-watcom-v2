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
            switch( ConstType ) {
            case TYP_UCHAR:
            case TYP_UINT:
            case TYP_ULONG:
                ultoa( U32Fetch( Constant64 ), Buffer, 10 );
                break;
            case TYP_SCHAR:
            case TYP_SINT:
            case TYP_SLONG:
                ltoa( U32Fetch( Constant64 ), Buffer, 10 );
                break;
            case TYP_ULONG64:
            case TYP_SLONG64:
                sti64cpy( Buffer, Constant64 );
                break;
            }
        }
    } else {
        strcpy( Buffer, Tokens[CurToken] );
    }

}

static PTREE genFnCall( NAME name )
{
    return( PTreeBinary( CO_CALL, PTreeId( name ), NULL ) );
}

static boolean endOfAsmStmt( void )
{
    if( CurToken == T_EOF ) return( TRUE );
    if( CurToken == T_NULL ) return( TRUE );
    if( CurToken == T___ASM ) return( TRUE );
    if( CurToken == T_RIGHT_BRACE ) return( TRUE );
    if( CurToken == T_ALT_RIGHT_BRACE ) return( TRUE );
    if( CurToken == T_SEMI_COLON ) return( TRUE );
    return( FALSE );
}

static void getAsmLine( VBUF *buff )
{
    char line[256];

    if( endOfAsmStmt() )
        return;
    /* reserve at least MAX_INSTR_SIZE bytes in the buffer */
    VbufReqd( buff, _RoundUp( ( AsmCodeAddress + MAX_INSTR_SIZE ), MAX_INSTR_SIZE ) );
    AsmCodeBuffer = VbufBuffer( buff );
    ensureBufferReflectsCurToken();
    if( IS_ID_OR_KEYWORD( CurToken ) && strcmp( Buffer, "__emit" ) == 0 ) {
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
        switch( CurToken ) {
        case T_ALT_XOR:
        case T_ALT_EXCLAMATION:
        case T_ALT_AND_AND:
        case T_ALT_OR_OR:
            strncat( line, " ", sizeof(line)-1 );
            break;
        default:
            if( IS_ID_OR_KEYWORD( CurToken ) )
                strncat( line, " ", sizeof(line)-1 );
            break;
        }
        NextToken();
        ensureBufferReflectsCurToken();
    }
    if( line[0] != '\0' ) {
        AsmSysLine( line );
    }
    VbufSetLen( buff, AsmCodeAddress );
    if( CurToken == T_SEMI_COLON ) {
        // ; .ASM comment
        for(;;) {
            NextToken();
            if( CurToken == T_EOF ) break;
            if( CurToken == T_NULL ) break;
        }
    }
}

TOKEN NextTokenSkipEOL( void )
/****************************/
{
    PPCTL_DISABLE_EOL();
    NextToken();
    PPCTL_ENABLE_EOL();
    return( CurToken );
}

PTREE AsmStmt( void )
/*******************/
{
    boolean     uses_auto;
    AUX_INFO    *auxinfo;
    TOKEN       skip_token;
    TOKEN       skip_alt_token;
    PTREE       expr;
    TYPE        fn_type;
    TYPE        ret_type;
    SYMBOL      sym;
    NAME        fn_name;
    auto VBUF   code_buffer;
    ppctl_t     old_ppctl;

    old_ppctl = PPControl;
    PPCTL_ENABLE_EOL();
    PPCTL_ENABLE_ASM();
    VbufInit( &code_buffer );
    NextTokenSkipEOL();
    AsmSysInit();
    if( ( CurToken == T_LEFT_BRACE ) || ( CurToken == T_ALT_LEFT_BRACE ) ) {
        NextTokenSkipEOL();
        for(;;) {
            getAsmLine( &code_buffer );
            if( CurToken == T_RIGHT_BRACE ) break;
            if( CurToken == T_ALT_RIGHT_BRACE ) break;
            if( CurToken == T_EOF ) break;
            NextTokenSkipEOL();
        }
        skip_token = T_RIGHT_BRACE;
        skip_alt_token = T_ALT_RIGHT_BRACE;
    } else {
        getAsmLine( &code_buffer );
        skip_token = skip_alt_token = T_NULL;
    }
    PPControl = old_ppctl;
    if( ( CurToken == skip_token ) || ( CurToken == skip_alt_token ) ) {
        NextToken();
    }
    if( AsmCodeAddress != 0 ) {
        fn_name = NameDummy();
        auxinfo = AsmSysCreateAux( fn_name );
        uses_auto = AsmSysInsertFixups( &code_buffer );
        if( uses_auto ) {
            AsmSysUsesAuto();
        }
        AsmSysDone();
        ret_type = GetBasicType( TYP_VOID );
        fn_type = MakeModifiableFunction( ret_type, NULL );
        fn_type->u.f.pragma = auxinfo;
        fn_type = CheckDupType( fn_type );
        sym = SymCreateFileScope( fn_type, SC_NULL, SF_NULL, fn_name );
        LinkageSet( sym, "C" );
        expr = genFnCall( fn_name );
    } else {
        expr = NULL;
    }
    AsmSysFini();
    VbufFree( &code_buffer );
    return( expr );
}

#endif
