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


#include "cvars.h"
#include "cgswitch.h"
#include "target.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "asinline.h"
#include "scan.h"

static  hw_reg_set      StackParms[] = { HW_D( HW_EMPTY ) };
static  hw_reg_set      AsmRegsSaved = { HW_D( HW_FULL ) };
static  int             AsmFuncNum;

#define MAX_NUM_INS     256

extern uint_32  *AsmCodeBuffer;
extern uint_32  AsmCodeAddress;

extern void *AsmAlloc( unsigned amount );

extern void AsmFree( void *ptr );





void AsmWarning( char *msg ) {
//****************************
// CC provides this

}

uint_32 AsmQuerySPOffsetOf( char *name ) {
//****************************************
// CC provides this
    return( 0 );
}

enum sym_state AsmQueryExternal( char *name )
{
    SYM_HANDLE sym_handle;
    auto SYM_ENTRY sym;

    sym_handle = SymLook( CalcHash( name, strlen( name ) ), name );
    if( sym_handle == 0 ) return( SYM_UNDEFINED );
    SymGet( &sym, sym_handle );
    switch( sym.stg_class ) {
    case SC_AUTO:
    case SC_REGISTER:
        return( SYM_STACK );
    }
    return( SYM_EXTERNAL );
}

local void FreeAsmFixups()
{
    asmreloc    *reloc, *next;
    for( reloc = AsmRelocs; reloc; reloc = next ) {
        next = reloc->next;
//      AsmFree( reloc->name );       this routine should be in asm
//      AsmFree( reloc );
    }
//  AsmRelocs = NULL;
}

static byte_seq_reloc *GetFixups( void ){
/****************************************/
    asmreloc       *reloc;
    byte_seq_reloc  *head,*new;
    byte_seq_reloc **lnk;
    SYM_HANDLE     sym_handle;

    head = NULL;
    lnk = &head;
    for( reloc = AsmRelocs; reloc; reloc = reloc->next ) {
        new = CMemAlloc( sizeof( byte_seq_reloc ) );
        sym_handle = SymLook( CalcHash( reloc->name, strlen( reloc->name ) ), reloc->name );
        new->off = reloc->offset;
        new->type = reloc->type;
        new->sym = (void *)sym_handle;
        new->next = NULL;
        *lnk = new;
        lnk = &new->next;
    }
    return( head );
}

#ifdef NEWCFE
static int EndOfAsmStmt( void )
{
    if( CurToken == T_EOF ) return( TRUE );
    if( CurToken == T_NULL ) return( TRUE );
    if( CurToken == T___ASM ) return( TRUE );
    if( CurToken == T_RIGHT_BRACE ) return( TRUE );
    if( CurToken == T_SEMI_COLON ) return( TRUE );
    return( FALSE );
}

static void AbsorbASMConstant( char *buff, unsigned size )
{
    // 0a0b3h is a valid .ASM constant
    for(;;) {
        NextToken();
        if( EndOfAsmStmt() || ( CharSet[ Buffer[0] ] & (C_AL|C_DI) ) == 0 ) {
            return;
        }
        strncat( buff, Buffer, size );
    }
}
static int IsId( unsigned token )
{
    if( token == T_ID ) {
        return( TRUE );
    }
    if( token >= FIRST_KEYWORD && token <= LAST_KEYWORD ) {
        return( TRUE );
    }
    return( FALSE );
}

void GetAsmLine()
{
    char        buf[256];

    CompFlags.pre_processing = 1;       // cause T_NULL token at end of line
    if( strcmp( Buffer, "_emit" ) == 0 ) {
        NextToken();                    // get numeric constant
        if( CurToken != T_CONSTANT ) {
            ExpectConstant();
        } else {
            AsmCodeBuffer[AsmCodeAddress++] = Constant;
            NextToken();
        }
    } else {
        buf[0] = '\0';
        for(;;) {
            if( EndOfAsmStmt() ) break;
            strncat( buf, Buffer, 255 );
            if( CurToken == T_CONSTANT ) {
                AbsorbASMConstant( buf, 255 );
                strncat( buf, " ", 255 );
            } else {
                if( IsId( CurToken ) ) {
                    NextToken();
                    if( CurToken != T_XOR ) {
                        strncat( buf, " ", 255 );
                    }
                } else {
                    NextToken();
                }
            }
        }
        if( buf[0] != '\0' ){
            AsmLine( buf );
        }
    }
}

void AsmStmt()
{
    int                 i;
    SYM_HANDLE          sym_handle;
    TREEPTR             tree;
    int                 too_many_bytes;
    int                 uses_auto;
    uint_32             buff[MAX_NUM_INS];
    auto char           name[8];

    // indicate that we are inside an __asm statement so scanner will
    // allow tokens unique to the assembler. e.g. 21h
    CompFlags.inside_asm_stmt = 1;
    NextToken();
    AsmInit();
    AsmCodeBuffer = &buff[0];
    AsmCodeAddress = 0;
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        too_many_bytes = 0;
        for(;;) {               // grab assembler lines
            GetAsmLine();
            if( AsmCodeAddress >= sizeof( buff ) ) {
                if( ! too_many_bytes ) {
                    CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                    too_many_bytes = 1;
                }
                AsmCodeAddress = 0;    // reset index to we don't overrun buffer
            }
            if( CurToken == T_RIGHT_BRACE ) break;
            if( CurToken == T_EOF ) break;
            if( CurToken == T_NULL ){  //skip over NL
                CompFlags.pre_processing = 0;
            }
            NextToken();
        }
        CompFlags.pre_processing = 0;
        CompFlags.inside_asm_stmt = 0;
        NextToken();
    } else {
        GetAsmLine();           // grab single assembler instruction
        CompFlags.pre_processing = 0;
        CompFlags.inside_asm_stmt = 0;
        if( CurToken == T_NULL ) {
            NextToken();
        }
    }
    i = AsmCodeAddress;
    if( i != 0 ) {
        sprintf( name, "F.%d", AsmFuncNum );
        ++AsmFuncNum;
        CreateAux( name );
        *CurrInfo = DefaultInfo;
        CurrInfo->use = 1;
        CurrInfo->save = AsmRegsSaved;  // indicate no registers saved
        if( too_many_bytes ) {
             uses_auto = 0;
        }else{
            risc_byte_seq *seq;

            seq = (risc_byte_seq *) CMemAlloc( sizeof(risc_byte_seq)+i );
            seq->relocs = GetFixups();
            seq->length = i;
            memcpy( &seq->data[0], buff, i );
            CurrInfo->code = seq;
        }
        FreeAsmFixups();
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
        sym_handle = MakeFunction( CStrSave( name ),
                        FuncNode( GetType( TYPE_VOID ), 0, NULL ) );
        tree = LeafNode( OPR_FUNCNAME );
        tree->op.sym_handle = sym_handle;
        tree = ExprNode( tree, OPR_CALL, NULL );
        tree->expr_type = GetType( TYPE_VOID );
        AddStmt( tree );
    }
    AsmFini();
}
#endif

local int GetByteSeq()
{
    uint_32             buff[MAX_NUM_INS];
    int                 i;
    int                 uses_auto;
    char                too_many_bytes;

    AsmInit();
    CompFlags.pre_processing = 1;       /* enable macros */
    NextToken();
    too_many_bytes = 0;
    i = 0;
    for(;;) {
        if( CurToken == T_STRING ) {    /* 06-sep-91 */
            AsmCodeBuffer = buff;
            AsmCodeAddress = i;
            AsmLine( Buffer );
            i = AsmCodeAddress;
            if( i >= sizeof( buff ) ) {
                if( ! too_many_bytes ) {
                    CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                    too_many_bytes = 1;
                }
                i = 0;          // reset index to we don't overrun buffer
            }
            NextToken();
            if( CurToken == T_COMMA )  NextToken();
        } else if( CurToken == T_CONSTANT ) {
            if( i < sizeof( buff ) ) {
                buff[ i++ ] = Constant;
            } else {
                if( ! too_many_bytes ) {
                    CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                    too_many_bytes = 1;
                }
            }
            NextToken();
        }else{
            break;
        }
    }
    if( too_many_bytes ) {
        uses_auto = 0;
    }else{
        risc_byte_seq *seq;

        seq = (risc_byte_seq *) CMemAlloc( sizeof(risc_byte_seq)+i );
        seq->relocs = GetFixups();
        seq->length = i;
        memcpy( &seq->data[0], buff, i );
        CurrInfo->code = seq;
    }
    FreeAsmFixups();
    CompFlags.pre_processing = 2;
    AsmFini();
    return( uses_auto );
}


void PragmaInit(void ){
/****************/
    AsmFuncNum = 0;
    PragInit();
}

static int GetAliasInfo( void ){
/*************************/
    auto char   buff[256];

    CurrAlias = &DefaultInfo;
    if( CurToken != T_LEFT_PAREN ) return( 1 );
    NextToken();
    if( CurToken != T_ID ) return( 0 );
    PragCurrAlias();
    strcpy( buff, Buffer );
    NextToken();
    if( CurToken == T_RIGHT_PAREN ) {
        NextToken();
        return( 1 );
    } else if( CurToken == T_COMMA ) {
        NextToken();
        if( CurToken != T_ID ) return( 0 );
        CreateAux( buff );
        PragCurrAlias();
        NextToken();
        if( CurToken == T_RIGHT_PAREN ) {
            *CurrInfo = *CurrAlias;
            NextToken();
        }
        PragEnding();
        return( 0 ); /* process no more! */
    } else {
        return( 0 ); // shut up the compiler
    }
}

static void GetPdata( void ){
/***************************/
    char *name;

    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_ID );
    } else {
        name = CStrSave( Buffer );
        CurrInfo->except_rtn = name;
        NextToken();
    }
}

void PragAux( void ){
/*************/
    struct {
        unsigned f_export : 1;
        unsigned f_parm   : 1;
        unsigned f_value  : 1;
        unsigned f_modify : 1;
        unsigned f_frame  : 1;
        unsigned f_except : 1;
        unsigned uses_auto: 1;
    } have;

    if( !GetAliasInfo() ) return;
    CurrEntry = NULL;
    if( CurToken != T_ID ) return;
    SetCurrInfo();
    NextToken();
    *CurrInfo = *CurrAlias;
    PragObjNameInfo();
    have.f_export = 0;
    have.f_parm   = 0;
    have.f_value  = 0;
    have.f_modify = 0;
    have.f_frame = 0;
    have.f_except = 0;
    have.uses_auto = 0; /* BBB - Jan 26, 1994 */
    for( ;; ) {
        if( CurToken == T_EQUAL ) {
            have.uses_auto = GetByteSeq();
        } else if( !have.f_export && PragRecog( "export" ) ) {
            CurrInfo->class |= DLL_EXPORT;
            have.f_export = 1;
        } else if( !have.f_parm && PragRecog( "parm" ) ) {
//          GetParmInfo();
            have.f_parm = 1;
        } else if( !have.f_value && PragRecog( "value" ) ) {
//          GetRetInfo();
            have.f_value = 1;
        } else if( !have.f_value && PragRecog( "aborts" ) ) {
            CurrInfo->class |= SUICIDAL;
            have.f_value = 1;
        } else if( !have.f_modify && PragRecog( "modify" ) ) {
//          GetSaveInfo();
            have.f_modify = 1;
        } else if( !have.f_frame && PragRecog( "frame" ) ) {
//          CurrInfo->class |= GENERATE_STACK_FRAME;
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
//      HW_CTurnOff( CurrInfo->save, HW_SP );
    }
    PragEnding();
}


hw_reg_set PragRegName( char *str )
    {
        return( StackParms[ 0 ] );
    }
