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
#include "memmgr.h"
#include "pragdefn.h"
#include "initdefs.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "fnovload.h"
#include "name.h"
#include "asmstmt.h"
#include "asinline.h"
#include "carve.h"
#include "pcheader.h"

static void pragmaInit(         // INIT PRAGMAS
    INITFINI* defn )            // - definition
{
    defn = defn;
    PragInit();
    if( ! CompFlags.dll_subsequent ) {
        AsmInit();
    }
}

static void pragmaFini(         // FINISH PRAGMAS
    INITFINI* defn )            // - definition
{
    defn = defn;
    AsmFini();
    CgInfoFreeLibs();
    CgInfoFreeImports();
}


INITDEFN( pragmas, pragmaInit, pragmaFini )


void PragAux(                   // #PRAGMA AUX ...
    void )
{
}


boolean PragmaChangeConsistent( // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma (old)
    AUX_INFO *newp )            // - pragma (new)
{
    if( oldp == NULL ) {
        oldp = &DefaultInfo;
    }
    if( oldp == newp ) {
        return TRUE;
    }
    return FALSE;
}

boolean PragmaOKForInlines(     // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    struct aux_info *fnp )      // - pragma
{
    if( fnp->code != NULL ) {
        return( FALSE );
    }
    return( TRUE );
}

boolean PragmaOKForVariables(   // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
{
    AUX_INFO *def_info;

    // only the obj_name can be set for variables everything else is
    // specific to functions except for "__cdecl"
    if( datap == &CdeclInfo ) {
        return( TRUE );
    }
    def_info = &DefaultInfo;
    if( datap->_class != def_info->_class ) {
        return( FALSE );
    }
    if( datap->code != def_info->code ) {
        return( FALSE );
    }
    if( datap->flags != def_info->flags ) {
        return( FALSE );
    }
    return( TRUE );
}


// The following defines which flags are to be ignored when checking
// a pragma call classes for equivalence.
//
#define CALL_CLASS_IGNORE ( 0                       \
                          | NO_MEMORY_CHANGED       \
                          | NO_MEMORY_READ          \
                          )

boolean PragmasTypeEquivalent(  // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
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
        return TRUE;
    }
    return
           ( ( inf1->_class & ~CALL_CLASS_IGNORE ) ==
             ( inf2->_class & ~CALL_CLASS_IGNORE ) )
        && ( inf1->flags == inf2->flags );
}

boolean AsmSysInsertFixups( VBUF *code )
/**************************************/
{
    SYMBOL sym;
    boolean uses_auto;
    asmreloc *curr;
    asmreloc *next;
    byte_seq_reloc *cg_relocs;
    byte_seq_reloc *new_reloc;
    risc_byte_seq *seq;

    seq = CMemAlloc( offsetof( risc_byte_seq, data ) + code->used );
    seq->length = code->used;
    memcpy( seq->data, code->buf, code->used );
    cg_relocs = NULL;
    uses_auto = FALSE;
    for( curr = AsmRelocs; curr != NULL; curr = next ) {
        next = curr->next;
        sym = ScopeASMUseSymbol( curr->name, &uses_auto );
        if( sym != NULL ) {
            new_reloc = CMemAlloc( sizeof( *new_reloc ) );
            new_reloc->off = curr->offset;
            new_reloc->type = curr->type;
            new_reloc->sym = sym;
            new_reloc->next = cg_relocs;
            cg_relocs = new_reloc;
        }
        AsmFree( curr );
    }
    AsmRelocs = NULL;
    seq->relocs = cg_relocs;
    CurrInfo->code = seq;
    return( uses_auto );
}

void *AsmSysCreateAux( char *name )
/**********************************/
{
    CreateAux( name );
    AuxCopy( CurrInfo, &DefaultInfo );
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
    AsmCodeAddress = 0;
}

char const *AsmSysDefineByte( void )
/**********************************/
{
    return( ".byte" );
}

void AsmSysDone( void )
/*********************/
{
    PragEnding( FALSE );
}

uint_32 AsmSysAddress( void )
/***************************/
{
    return AsmCodeAddress;
}

void AsmSysSetCodeBuffer( void *buff )
/************************************/
{
    AsmCodeBuffer = buff;
}

void AsmSysParseLine( char *line )
/********************************/
{
    AsmLine( line );
}

enum sym_state AsmQueryExternal( char *name )
/*******************************************/
{
    SYMBOL sym;
    enum sym_state state;

    state = SYM_UNDEFINED;
    sym = ScopeASMLookup( name );
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
    SYMBOL sym;
    byte_seq_reloc *reloc;
    risc_byte_seq *code;
    unsigned code_size;

    code = info->code;
    if( code == NULL ) {
        /* no code */
        PCHWriteUInt( 0 );
        return;
    }
    code_size = offsetof( risc_byte_seq, data ) + code->length;
    PCHWriteUInt( code_size );
    PCHWrite( code, code_size );
    for( reloc = code->relocs; reloc != NULL; reloc = reloc->next ) {
        sym = SymbolGetIndex( reloc->sym );
        PCHWrite( &sym, sizeof( sym ) );
        PCHWrite( &reloc->off, sizeof( reloc->off ) );
        PCHWrite( &reloc->type, sizeof( reloc->type ) );
    }
    sym = SymbolGetIndex( NULL );
    PCHWrite( &sym, sizeof( sym ) );
}

void AsmSysPCHReadCode( AUX_INFO *info )
/**************************************/
{
    cv_index si;
    byte_seq_reloc *reloc;
    byte_seq_reloc **head;
    risc_byte_seq *code;
    unsigned code_size;

    code_size = PCHReadUInt();
    if( code_size == 0 ) {
        info->code = NULL;
        return;
    }
    code = CMemAlloc( code_size );
    info->code = code;
    PCHRead( code, code_size );
    code->relocs = NULL;
    head = &(code->relocs);
    for(;;) {
        PCHLocateCVIndex( si );
        if( si == CARVE_NULL_INDEX ) break;
        reloc = CMemAlloc( sizeof( *reloc ) );
        reloc->next = NULL;
        *head = reloc;
        head = &(reloc->next);
        reloc->sym = SymbolMapIndex( (SYMBOL) si );
        PCHRead( &reloc->off, sizeof( reloc->off ) );
        PCHRead( &reloc->type, sizeof( reloc->type ) );
    }
}

syscode_seq *AuxCodeDup(        // DUPLICATE AUX CODE
    syscode_seq *code )
{
    byte_seq_len code_length;
    byte_seq_len size;
    risc_byte_seq *new_code;
    byte_seq_reloc *reloc;
    byte_seq_reloc *new_reloc;

    if( code == NULL ) {
        return( code );
    }
    code_length = code->length;
    size = offsetof( risc_byte_seq, data ) + code_length;
    new_code = (risc_byte_seq *)vctsave( (char *)code, size );
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

void AsmSysCopyCode( void )
/*************************/
{
    risc_byte_seq *code;

    code = CurrInfo->code;
    if( code != NULL && code == CurrAlias->code ) {
        CurrInfo->code = AuxCodeDup( code );
    }
}
