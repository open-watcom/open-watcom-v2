/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS target specific pragma processing.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pdefn2.h"
#include "asmstmt.h"
#include "scan.h"
#include <ctype.h>

static  hw_reg_set      AsmRegsSaved = HW_D( HW_FULL );
static  int             AsmFuncNum;
static  aux_info        AuxInfo;

static struct {
    boolbit     f_returns   : 1;
//    boolbit     f_streturn  : 1;
} AuxInfoFlg;


uint_32 AsmQuerySPOffsetOf( void *handle )
/****************************************/
{
    /* unused parameters */ (void)handle;

    // CC provides this
    return( 0 );
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
    for( reloc = AsmRelocs; reloc != NULL; reloc = reloc->next ) {
        sym_handle = SymLook( CalcHash( reloc->name, strlen( reloc->name ) ), reloc->name );
        if( sym_handle == SYM_NULL ) {
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
//            uses_auto = true;
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

static bool GetByteSeq( byte_seq **code )
/***************************************/
{
    unsigned char       buff[MAXIMUM_BYTESEQ + 32];
    bool                uses_auto;
    bool                too_many_bytes;

    AsmSysInit( buff );
    NextToken();
    too_many_bytes = false;
    uses_auto = false;
    for( ;; ) {
        if( CurToken == T_STRING ) {
            AsmLine( Buffer );
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
            }
        } else if( CurToken == T_CONSTANT ) {
            AsmCodeBuffer[AsmCodeAddress++] = (unsigned char)Constant;
            NextToken();
        } else {
            break;
        }
        if( AsmCodeAddress > MAXIMUM_BYTESEQ ) {
            if( !too_many_bytes ) {
                CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                too_many_bytes = true;
            }
            AsmCodeAddress = 0; // reset index to we don't overrun buffer
        }
    }
    AsmFini();
    if( too_many_bytes ) {
        uses_auto = false;
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
{
    CurrAlias = NULL;
    CurrInfo = NULL;
    CurrEntry = NULL;

    memset( &AuxInfo, 0, sizeof( AuxInfo ) );

    AuxInfoFlg.f_returns = false;
//    AuxInfoFlg.f_streturn = false;
}

static void PragmaAuxEnd( void )
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

    if( !HW_CEqual( AuxInfo.save, HW_EMPTY ) ) {
        HW_TurnOff( CurrInfo->save, AuxInfo.save );
    }
    PragmaAuxEnding();
}

bool GetPragmaAuxAlias( void )
/****************************/
{
    CurrAlias = PragmaAuxAlias( Buffer );
    NextToken();
    if( CurToken == T_RIGHT_PAREN )
        NextToken();
    PragmaAuxEnd();
    return( true );
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
    AuxInfoFlg.f_returns = true;
    AuxInfo.returns = PragRegList();
}


void PragAux( void )
/******************/
{
    struct {
        boolbit     f_export    : 1;
        boolbit     f_parm      : 1;
        boolbit     f_value     : 1;
        boolbit     f_modify    : 1;
        boolbit     f_frame     : 1;
        boolbit     f_except    : 1;
        boolbit     uses_auto   : 1;
    } have;

    InitAuxInfo();
    PPCTL_ENABLE_MACROS();
    NextToken();
    if( GetPragmaAuxAliasInfo() ) {
        SetCurrInfo( Buffer );
        NextToken();
        PragObjNameInfo( &AuxInfo.objname );
        have.f_export = false;
        have.f_parm = false;
        have.f_value = false;
        have.f_modify = false;
        have.f_frame = false;
        have.f_except = false;
        have.uses_auto = false;
        for( ;; ) {
            if( CurToken == T_EQUAL ) {
                have.uses_auto = GetByteSeq( &AuxInfo.code );
            } else if( !have.f_export && PragRecogId( "export" ) ) {
                AuxInfo.cclass |= DLL_EXPORT;
                have.f_export = true;
            } else if( !have.f_parm && PragRecogId( "parm" ) ) {
                GetParmInfo();
                have.f_parm = true;
            } else if( !have.f_value && PragRecogId( "value" ) ) {
                GetRetInfo();
                have.f_value = true;
            } else if( !have.f_value && PragRecogId( "aborts" ) ) {
                AuxInfo.cclass |= SUICIDAL;
                have.f_value = true;
            } else if( !have.f_modify && PragRecogId( "modify" ) ) {
                GetSaveInfo();
                have.f_modify = true;
            } else if( !have.f_frame && PragRecogId( "frame" ) ) {
//                AuxInfo.cclass |= GENERATE_STACK_FRAME;
                have.f_frame = true;
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
//            HW_CTurnOff( AuxInfo.save, HW_SP );
        }
        PragmaAuxEnd();
    }
    PPCTL_DISABLE_MACROS();
}


hw_reg_set PragRegName( const char *regname, size_t regnamelen )
/**************************************************************/
{
    int             index;
    const char      *str;
    hw_reg_set      name;
    size_t          len;

    if( regnamelen > 0 ) {
        len = regnamelen;
        str = SkipUnderscorePrefix( regname, &len, false );
        if( len > 0 && *str == '$' ) {
            str++;
            len--;
            if( len > 0 ) {
                // search register or alias name
                index = PragRegIndex( Registers, str, len, false );
                if( index != -1 ) {
                    return( RegBits[RegMap[index]] );
                }
                // decode regular register index
                index = PragRegNumIndex( str, len, 32 );
                if( index != -1 ) {
                    return( RegBits[index] );
                }
            }
        }
        PragRegNameErr( regname, regnamelen );
    }
    HW_CAsgn( name, HW_EMPTY );
    return( name );
}

hw_reg_set PragReg( void )
{
    char            buffer[20];
    size_t          len;
    bool            prefix;

    prefix = ( CurToken == T_BAD_CHAR && Buffer[0] == '$' );
    if( prefix ) {
        buffer[0] = '$';
        NextToken();
    }
    len = TokenLen;
    if( prefix )
        len++;
    if( len > sizeof( buffer ) - 1 ) {
        len = sizeof( buffer ) - 1;
    }
    if( prefix ) {
        memcpy( buffer + 1, Buffer, len - 1 );
    } else {
        memcpy( buffer, Buffer, len );
    }
    buffer[len] = '\0';
    return( PragRegName( buffer, len ) );
}

void AsmSysInit( unsigned char *buf )
/***********************************/
{
    AsmInit();
    AsmCodeBuffer = buf;
    AsmCodeLimit = MAXIMUM_BYTESEQ;
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
    char                name[8];

    AsmFini();
    uses_auto = false;
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
            uses_auto = false;
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
        sym_handle = MakeFunction( name, FuncNode( GetType( TYP_VOID ), FLAG_NONE, NULL ) );
        tree = LeafNode( OPR_FUNCNAME );
        tree->op.u2.sym_handle = sym_handle;
        tree = ExprNode( tree, OPR_CALL, NULL );
        tree->u.expr_type = GetType( TYP_VOID );
        AddStmt( tree );
    }
    AsmFiniRelocs();
}


char const *AsmSysDefineByte( void )
/**********************************/
{
    return( ".byte " );
}
