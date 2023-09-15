/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "memmgr.h"
#include "pragdefn.h"
#include "initdefs.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "fnovload.h"
#include "name.h"
#include "asmstmt.h"
#include "carve.h"
#include "pcheader.h"


//
// The following defines which flags are to be ignored when checking
// a pragma call classes for equivalence.
//
#define FECALL_GEN_CALL_CLASS_IGNORE ( 0 \
    | FECALL_GEN_NO_MEMORY_CHANGED       \
    | FECALL_GEN_NO_MEMORY_READ          \
    | FECALL_GEN_DLL_EXPORT              \
)

static void pragmasInit(        // INIT PRAGMAS
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    PragmaAuxInit();
    SetDefaultAuxInfo();
}

static void pragmasFini(        // FINISH PRAGMAS
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    CgInfoFreeLibs();
    CgInfoFreeImports();
    CgInfoFreeImportsS();
}


INITDEFN( pragmas, pragmasInit, pragmasFini )


static void assemblerInit(      // INIT ASSEMBLER
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    if( !CompFlags.dll_subsequent ) {
        AsmInit();
    }
}

static void assemblerFini(      // FINISH ASSEMBLER
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    AsmFini();
}


INITDEFN( assembler, assemblerInit, assemblerFini )


bool PragmaChangeConsistent(    // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma (old)
    AUX_INFO *newp )            // - pragma (new)
{
    if( oldp == NULL ) {
        oldp = &DefaultInfo;
    }
    if( oldp == newp ) {
        return( true );
    }
    return( false );
}

bool PragmaOKForInlines(        // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    AUX_INFO *fnp )             // - pragma
{
    if( fnp->code != NULL ) {
        return( false );
    }
    return( true );
}

bool PragmaOKForVariables(      // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
{
    AUX_INFO *def_info;

    // only the obj_name can be set for variables everything else is
    // specific to functions except for "__cdecl"
    if( datap == &CdeclInfo ) {
        return( true );
    }
    def_info = &DefaultInfo;
    if( datap->cclass != def_info->cclass ) {
        return( false );
    }
    if( datap->code != def_info->code ) {
        return( false );
    }
    if( datap->flags != def_info->flags ) {
        return( false );
    }
    return( true );
}

//
// The following defines which flags are to be ignored when checking
// a pragma call classes for equivalence.
//

bool PragmasTypeEquivalent(     // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
    AUX_INFO *inf1,             // - pragma [1]
    AUX_INFO *inf2 )            // - pragma [2]
{
    if( inf1 == NULL ) {
        inf1 = &DefaultInfo;
    }
    if( inf2 == NULL ) {
        inf2 = &DefaultInfo;
    }
    if( inf1 == inf2 ) {
        return( true );
    }
    return
           ( ( inf1->cclass & ~FECALL_GEN_CALL_CLASS_IGNORE ) ==
             ( inf2->cclass & ~FECALL_GEN_CALL_CLASS_IGNORE ) )
        && ( inf1->flags == inf2->flags );
}

static byte_seq *AuxCodeDup(        // DUPLICATE AUX CODE
    byte_seq *code )
{
    byte_seq_len code_length;
    byte_seq_len size;
    byte_seq *new_code;
    byte_seq_reloc *reloc;
    byte_seq_reloc *new_reloc;

    if( code == NULL ) {
        return( code );
    }
    code_length = code->length;
    size = offsetof( byte_seq, data ) + code_length;
    new_code = (byte_seq *)vctsave( (char *)code, size );
    new_code->relocs = NULL;
    for( reloc = code->relocs; reloc != NULL; reloc = reloc->next ) {
        new_reloc = CMemAlloc( sizeof( *new_reloc ) );
        new_reloc->next = new_code->relocs;
        new_reloc->sym = reloc->sym;
        new_reloc->off = reloc->off;
        new_reloc->type = reloc->type;
        new_code->relocs = new_reloc;
    }
    return( new_code );
}

static void AuxCopy(           // COPY AUX STRUCTURE
    AUX_INFO *to,               // - destination
    AUX_INFO *from )            // - source
{
    freeAuxInfo( to );
    *to = *from;
    to->parms = AuxParmDup( from->parms );
    to->objname = AuxObjnameDup( from->objname );
    to->code = AuxCodeDup( from->code );
}

void GetPragmaAuxAlias( void )
/****************************/
{
    CurrAlias = PragmaAuxAlias( Buffer );
    NextToken();
    if( CurToken == T_RIGHT_PAREN ) {
        AuxCopy( CurrInfo, CurrAlias );
        NextToken();
    }
}

void PragAux(                   // #PRAGMA AUX ...
    void )
/***********/
{
}

bool AsmSysInsertFixups( VBUF *code )
/***********************************/
{
    SYMBOL          sym;
    bool            uses_auto;
    asmreloc        *curr;
    byte_seq_reloc  **lnk;
    byte_seq_reloc  *new_reloc;
    byte_seq        *seq;
    unsigned        len;

    len = VbufLen( code );
    seq = CMemAlloc( offsetof( byte_seq, data ) + len );
    seq->length = len;
    memcpy( seq->data, VbufBuffer( code ), len );
    lnk = &seq->relocs;
    uses_auto = false;
    for( curr = AsmRelocs; curr != NULL; curr = curr->next ) {
        sym = ScopeASMUseSymbol( NameCreateNoLen( curr->name ), &uses_auto );
        if( sym != NULL ) {
            new_reloc = CMemAlloc( sizeof( *new_reloc ) );
            new_reloc->off = curr->offset;
            new_reloc->type = curr->type;
            new_reloc->sym = sym;
            *lnk = new_reloc;
            lnk = &new_reloc->next;
        }
    }
    AsmRelocs = NULL;
    *lnk = NULL;
    CurrInfo->code = seq;
    return( uses_auto );
}

AUX_INFO *AsmSysCreateAux( const char *name )
/*******************************************/
{
    CreateAux( name );
    AuxCopy( CurrInfo, &WatcallInfo );
    CurrInfo->use = 1;
    CurrEntry->info = CurrInfo;
    return( CurrInfo );
}

void AsmSysUsesAuto( void )
/*************************/
{
    ScopeASMUsesAuto();
}

void AsmSysInit( void )
/*********************/
{
    AsmInit();
    AsmCodeAddress = 0;
}

void AsmSysFini( void )
/*********************/
{
    AsmFiniRelocs();
    AsmFini();
}

char const *AsmSysDefineByte( void )
/**********************************/
{
    return( ".byte" );
}

void AsmSysDone( void )
/*********************/
{
    PragmaAuxEnding( false );
}

void *AsmQuerySymbol( const char *name )
/**************************************/
{
    return( ScopeASMLookup( NameCreateNoLen( name ) ) );
}

enum sym_state AsmQueryState( void *handle )
/******************************************/
{
    SYMBOL sym = handle;
    enum sym_state state;

    state = SYM_UNDEFINED;
    if( sym != NULL ) {
        if( SymIsAutomatic( sym ) ) {
            state = SYM_STACK;
        } else {
            state = SYM_EXTERNAL;
        }
    }
    return( state );
}

void AsmSysPCHWriteCode( AUX_INFO *info )
/***************************************/
{
    byte_seq_reloc *reloc;
    byte_seq *code;
    unsigned code_size;

    code = info->code;
    if( code == NULL ) {
        /* no code */
        PCHWriteUInt( 0 );
        return;
    }
    code_size = offsetof( byte_seq, data ) + code->length;
    PCHWriteUInt( code_size );
    PCHWrite( code, code_size );
    for( reloc = code->relocs; reloc != NULL; reloc = reloc->next ) {
        SymbolPCHWrite( reloc->sym );
        PCHWriteVar( reloc->off );
        PCHWriteVar( reloc->type );
    }
    PCHWriteCVIndexTerm();
}

void AsmSysPCHReadCode( AUX_INFO *info )
/**************************************/
{
    SYMBOL sym;
    byte_seq_reloc *reloc;
    byte_seq_reloc **head;
    byte_seq *code;
    unsigned code_size;

    code_size = PCHReadUInt();
    if( code_size == 0 ) {
        info->code = NULL;
        return;
    }
    code = CMemAlloc( code_size );
    info->code = code;
    PCHRead( code, code_size );
    head = &(code->relocs);
    for( ; (sym = SymbolPCHRead()) != NULL; ) {
        reloc = CMemAlloc( sizeof( *reloc ) );
        reloc->sym = sym;
        PCHReadVar( reloc->off );
        PCHReadVar( reloc->type );
        *head = reloc;
        head = &(reloc->next);
    }
    *head = NULL;
}

void AsmSysLine( const char *buff )
{
    AsmLine( buff );
}

void AsmSysCopyCode( void )
/*************************/
{
    byte_seq *code;

    code = CurrInfo->code;
    if( code != NULL && code == CurrAlias->code ) {
        CurrInfo->code = AuxCodeDup( code );
    }
}
