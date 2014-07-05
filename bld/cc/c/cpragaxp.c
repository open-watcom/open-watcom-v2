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
* Description:  Alpha AXP target specific pragma processing.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "asmstmt.h"
#include "scan.h"
#include <ctype.h>

static  hw_reg_set      AsmRegsSaved = HW_D( HW_FULL );
static  int             AsmFuncNum;
static  aux_info        AuxInfo;

static struct {
    unsigned    f_returns : 1;
//    unsigned    f_streturn : 1;
} AuxInfoFlg;


void *AsmQuerySymbol( const char *name )
/**************************************/
{
    return( SymLook( CalcHash( name, strlen( name ) ), name ) );
}


uint_32 AsmQuerySPOffsetOf( void *handle )
/****************************************/
{
// CC provides this
    return( 0 );
}


enum sym_state AsmQueryState( void *handle )
/******************************************/
{
    SYM_HANDLE sym_handle = (SYM_HANDLE)handle;
    auto SYM_ENTRY sym;

    if( sym_handle == 0 )
        return( SYM_UNDEFINED );
    SymGet( &sym, sym_handle );
    if( (sym.flags & SYM_REFERENCED) == 0 ) {
        sym.flags |= SYM_REFERENCED;
        SymReplace( &sym, sym_handle );
    }
    switch( sym.attribs.stg_class ) {
    case SC_AUTO:
    case SC_REGISTER:
        return( SYM_STACK );
    }
    return( SYM_EXTERNAL );
}


static byte_seq_reloc *GetFixups( void )
/**************************************/
{
    asmreloc            *reloc;
    byte_seq_reloc      *head;
    byte_seq_reloc      *new;
    byte_seq_reloc      **lnk;
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;

    lnk = &head;
    for( reloc = AsmRelocs; reloc; reloc = reloc->next ) {
        sym_handle = SymLook( CalcHash( reloc->name, strlen( reloc->name ) ), reloc->name );
        if( sym_handle == 0 ) {
            CErr2p( ERR_UNDECLARED_SYM, reloc->name );
            return( NULL );
        }
        SymGet( &sym, sym_handle );
        sym.flags |= SYM_REFERENCED | SYM_ADDR_TAKEN;
        switch( sym.attribs.stg_class ) {
        case SC_REGISTER:
        case SC_AUTO:
            sym.flags |= SYM_USED_IN_PRAGMA;
            CurFuncNode->op.u2.func.flags &= ~FUNC_OK_TO_INLINE;
//            uses_auto = TRUE;
            break;
        }
        SymReplace( &sym, sym_handle );
        new = CMemAlloc( sizeof( byte_seq_reloc ) );
        new->off = reloc->offset;
        new->type = reloc->type;
        new->sym = (void *)sym_handle;
        *lnk = new;
        lnk = &new->next;
    }
    *lnk = NULL;
    return( head );
}

void AsmSysLine( const char *buff )
/*********************************/
{
    AsmLine( buff );
}

local bool GetByteSeq( byte_seq **code )
/**************************************/
{
    unsigned char       buff[MAXIMUM_BYTESEQ + 32];
    bool                uses_auto;
    bool                too_many_bytes;

    AsmSysInit( buff );
    PPCTL_ENABLE_MACROS();
    NextToken();
    too_many_bytes = FALSE;
    uses_auto = FALSE;
    for( ;; ) {
        if( CurToken == T_STRING ) {
            AsmLine( Buffer );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
            }
        } else if( CurToken == T_CONSTANT ) {
            AsmCodeBuffer[AsmCodeAddress++] = Constant;
            NextToken();
        } else {
            break;
        }
        if( AsmCodeAddress > MAXIMUM_BYTESEQ ) {
            if( !too_many_bytes ) {
                CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                too_many_bytes = TRUE;
            }
            AsmCodeAddress = 0; // reset index to we don't overrun buffer
        }
    }
    PPCTL_DISABLE_MACROS();
    AsmFini();
    if( too_many_bytes ) {
        uses_auto = FALSE;
    } else {
        byte_seq      *seq;
        byte_seq_len  len;

        len = AsmCodeAddress;
        seq = (byte_seq *)CMemAlloc( offsetof( byte_seq, data ) + len );
        seq->relocs = GetFixups();
        seq->length = len;
        memcpy( &seq->data[0], buff, len );
        *code = seq;
    }
    AsmFiniRelocs();
    AsmSysFini();
    return( uses_auto );
}


void PragmaInit( void )
/*********************/
{
    AsmFuncNum = 0;
}

void PragmaFini( void )
/*********************/
{
}

static void InitAuxInfo( void )
/*****************************/
{
    CurrAlias   = NULL;
    CurrInfo    = NULL;
    CurrEntry   = NULL;

    memset( &AuxInfo, 0, sizeof( AuxInfo ) );

    AuxInfoFlg.f_returns    = 0;
//    AuxInfoFlg.f_streturn   = 0;
;
}

static void AdvanceToken( void )
/******************************/
{
    CMemFree( SavedId );
    SavedId = NULL;
    CurToken = LAToken;
}

static void CopyAuxInfo( void )
/*****************************/
{
    if( CurrEntry == NULL ) {
        // Redefining a built-in calling convention
    } else {
        CurrInfo = (aux_info *)CMemAlloc( sizeof( aux_info ) );
        *CurrInfo = *CurrAlias;
    }
    if( AuxInfo.code != NULL ) {
        CurrInfo->code = AuxInfo.code;
    }
    CurrInfo->cclass |= AuxInfo.cclass;
    if( AuxInfo.objname != NULL )
        CurrInfo->objname = AuxInfo.objname;
    if( AuxInfoFlg.f_returns )
        CurrInfo->returns = AuxInfo.returns;
//    if( AuxInfoFlg.f_streturn )
//        CurrInfo->streturn = AuxInfo.streturn;
    if( AuxInfo.parms != NULL )
        CurrInfo->parms = AuxInfo.parms;
    if( AuxInfo.except_rtn != NULL )
        CurrInfo->except_rtn = AuxInfo.except_rtn;
    if( !HW_CEqual( AuxInfo.save, HW_EMPTY ) ) {
        HW_TurnOff( CurrInfo->save, AuxInfo.save );
    }
}

static bool GetAliasInfo( void )
/******************************/
{
    if( CurToken != T_LEFT_PAREN )          // #pragma aux symbol .....
        return( TRUE );
    NextToken();
    if( CurToken != T_ID )                  // error
        return( FALSE );
    LookAhead();
    if( LAToken == T_RIGHT_PAREN ) {        // #pragma aux (alias) symbol .....
        PragCurrAlias( SavedId );
        AdvanceToken();
        NextToken();
        return( TRUE );
    } else if( LAToken == T_COMMA ) {       // #pragma aux (symbol, alias)
        HashValue = SavedHash;
        SetCurrInfo( SavedId );
        AdvanceToken();
        NextToken();
        if( CurToken != T_ID )              // error
            return( FALSE );
        PragCurrAlias( Buffer );
        NextToken();
        if( CurToken == T_RIGHT_PAREN )
            NextToken();
        CopyAuxInfo();
        PragEnding();
        return( FALSE ); /* process no more! */
    } else {                                // error
        AdvanceToken();
        return( FALSE ); // shut up the compiler
    }
}


static void GetPdata( void )
/**************************/
{
    char *name;

    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_ID );
    } else {
        name = CStrSave( Buffer );
        AuxInfo.except_rtn = name;
        NextToken();
    }
}


static void GetParmInfo( void )
/*****************************/
{
    if( PragRegSet() != T_NULL ) {
        AuxInfo.parms = PragManyRegSets();
    }
}


static void GetSaveInfo( void )
/*****************************/
{
    hw_reg_set      reg;

    reg = PragRegList();
    HW_TurnOn( AuxInfo.save, reg );
}


static void GetRetInfo( void )
/****************************/
{
    AuxInfoFlg.f_returns = 1;
    AuxInfo.returns = PragRegList();
}


void PragAux( void )
/******************/
{
    struct {
        unsigned f_export : 1;
        unsigned f_parm   : 1;
        unsigned f_value  : 1;
        unsigned f_modify : 1;
        unsigned f_frame  : 1;
        unsigned f_except : 1;
        unsigned uses_auto: 1;
    } have;

    InitAuxInfo();
    if( GetAliasInfo() && CurToken == T_ID ) {
        SetCurrInfo( Buffer );
        NextToken();
        PragObjNameInfo( &AuxInfo.objname );
        have.f_export = 0;
        have.f_parm   = 0;
        have.f_value  = 0;
        have.f_modify = 0;
        have.f_frame = 0;
        have.f_except = 0;
        have.uses_auto = 0;
        for( ;; ) {
            if( CurToken == T_EQUAL ) {
                have.uses_auto = GetByteSeq( &AuxInfo.code );
            } else if( !have.f_export && PragRecog( "export" ) ) {
                AuxInfo.cclass |= DLL_EXPORT;
                have.f_export = 1;
            } else if( !have.f_parm && PragRecog( "parm" ) ) {
                GetParmInfo();
                have.f_parm = 1;
            } else if( !have.f_value && PragRecog( "value" ) ) {
                GetRetInfo();
                have.f_value = 1;
            } else if( !have.f_value && PragRecog( "aborts" ) ) {
                AuxInfo.cclass |= SUICIDAL;
                have.f_value = 1;
            } else if( !have.f_modify && PragRecog( "modify" ) ) {
                GetSaveInfo();
                have.f_modify = 1;
            } else if( !have.f_frame && PragRecog( "frame" ) ) {
//                    AuxInfo.cclass |= GENERATE_STACK_FRAME;
                have.f_frame = 1;
            } else if( !have.f_except && PragRecog( "pdata_exception" ) ) {
                GetPdata();
                have.f_except = 1;
            } else {
                break;
            }
        }
        if( have.uses_auto ) {
            /*
                We want to force the calling routine to set up a [E]BP frame
                for the use of this pragma. This is done by saying the pragma
                modifies the [E]SP register. A kludge, but it works.
            */
//                HW_CTurnOn( AuxInfo.save, HW_SP );
        }
        CopyAuxInfo();
        PragEnding();
    }
}


hw_reg_set PragRegName( const char *strreg, size_t len )
/******************************************************/
{
    int             index;
    char            *str;
    hw_reg_set      name;
    char            buffer[20];

    if( len != 0 ) {
        if( *strreg == '_' ) {
            ++strreg;
            --len;
            if( *strreg == '_' ) {
                ++strreg;
                --len;
            }
        }
        if( len > ( sizeof( buffer ) - 1 ) )
            len = sizeof( buffer ) - 1;
        str = memcpy( buffer, strreg, len );
        str[len] = '\0';
        if( *str == '$' ) {
            ++str;
            --len;
            // search register or alias name
            index = PragRegIndex( Registers, str, len, FALSE );
            if( index != -1 ) {
                return( RegBits[RegMap[index]] );
            }
            // decode regular register index
            index = PragRegNumIndex( str, 32 );
            if( index != -1 ) {
                return( RegBits[index] );
            }
            --str;
        }
        CErr2p( ERR_BAD_REGISTER_NAME, str );
    }
    HW_CAsgn( name, HW_EMPTY );
    return( name );
}


void AsmSysInit( unsigned char *buf )
/***********************************/
{
    AsmInit();
    AsmCodeBuffer = buf;
    AsmCodeAddress = 0;
}


void AsmSysFini( void )
/*********************/
{
    AsmFini();
}


void AsmSysMakeInlineAsmFunc( bool too_many_bytes )
/*************************************************/
{
    byte_seq_len        code_length;
    SYM_HANDLE          sym_handle;
    TREEPTR             tree;
    bool                uses_auto;
    auto char           name[8];

    AsmFini();
    uses_auto = FALSE;
    code_length = AsmCodeAddress;
    if( code_length != 0 ) {
        sprintf( name, "F.%d", AsmFuncNum );
        ++AsmFuncNum;
        CreateAux( name );
        CurrInfo = (aux_info *)CMemAlloc( sizeof( aux_info ) );
        *CurrInfo = WatcallInfo;
        CurrInfo->use = 1;
        CurrInfo->save = AsmRegsSaved;  // indicate no registers saved
        if( too_many_bytes ) {
            uses_auto = FALSE;
        } else {
            byte_seq    *seq;

            seq = (byte_seq *)CMemAlloc( offsetof( byte_seq, data ) + code_length );
            seq->relocs = GetFixups();
            seq->length = code_length;
            memcpy( &seq->data[0], AsmCodeBuffer, code_length );
            CurrInfo->code = seq;
        }
        if( uses_auto ) {
            /*
               We want to force the calling routine to set up a [E]BP frame
               for the use of this pragma. This is done by saying the pragma
               modifies the [E]SP register. A kludge, but it works.
            */
//          HW_CTurnOff( CurrInfo->save, HW_SP );
        }
        CurrEntry->info = CurrInfo;
        CurrEntry->next = AuxList;
        AuxList = CurrEntry;
        CurrEntry = NULL;
        sym_handle = MakeFunction( name, FuncNode( GetType( TYPE_VOID ), 0, NULL ) );
        tree = LeafNode( OPR_FUNCNAME );
        tree->op.u2.sym_handle = sym_handle;
        tree = ExprNode( tree, OPR_CALL, NULL );
        tree->u.expr_type = GetType( TYPE_VOID );
        AddStmt( tree );
    }
    AsmFiniRelocs();
}


char const *AsmSysDefineByte( void )
/**********************************/
{
    return( ".byte " );
}
