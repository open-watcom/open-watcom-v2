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


#include <stdlib.h>
#include <string.h>
#include "asmsym.h"//
#include "asmops2.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmglob.h"

#ifdef _WASM_

#include "myassert.h"
#include "directiv.h"

#define Address         ( GetCurrAddr() )

#else

extern long     Address;
struct asm_sym  *AsmSymHead;

#endif

extern void     AsmError( int );



static unsigned short CvtTable[] = {
    T_BYTE,
    T_WORD,
    T_DWORD,
    T_DWORD,    /* should be T_PWORD, T_FWORD */
    T_DWORD,
    T_QWORD,
    T_TBYTE,
    T_NEAR,
    T_NEAR,
    T_FAR,
    T_FAR,
};

#ifdef _WASM_

#include "hash.h"
struct asm_sym *sym_table[ HASH_TABLE_SIZE ] = { NULL };
/* initialize the whole table to null pointers */

#endif

struct asm_sym **AsmFind( char *name )
/***********************************/
/* find a symbol in the symbol table, return NULL if not found */
{
    struct asm_sym      **sym;
    #ifdef _WASM_
        sym = &sym_table[ hashpjw( name ) ];
    #else
        sym = &AsmSymHead;
    #endif

    for( ; *sym; sym = &((*sym)->next) ) {
        if( stricmp( name, (*sym)->name ) == 0 ) return( sym );
    }
    return( sym );
}

struct asm_sym *AsmLookup( char *name )
/*************************************/
{
    struct asm_sym      **sym_ptr;
    struct asm_sym      *sym;
    char                is_current_loc = FALSE;

    if( strlen( name ) > MAX_ID_LEN ) {
        AsmError( LABEL_TOO_LONG );
        return NULL;
    }

    sym_ptr = AsmFind( name );
#ifdef _WASM_
    if( strcmp( name, "$" ) ==  0 ) {
        is_current_loc = TRUE;
        /* current address operator */
        if( *sym_ptr != NULL ) {
            GetSymInfo( *sym_ptr );
        }
    }
#endif
    if( *sym_ptr != NULL ) return( *sym_ptr );

#ifndef _WASM_
    sym = AsmAlloc( sizeof( struct asm_sym ) );
#else
    sym = AsmAlloc( sizeof( dir_node ) );
#endif
    if( sym != NULL ) {

#ifndef _WASM_
        sym->addr = Address;
#else
        sym->grpidx = 0;
        sym->segidx = 0;
        sym->public = FALSE;
        if( is_current_loc ) {
            GetSymInfo( sym );
            sym->state = SYM_INTERNAL;
        } else {
            sym->offset = 0;
        }
        sym->first_size = 0;
        sym->first_length = 0;
        sym->total_size = 0;
        sym->total_length = 0;
        sym->mangler = NULL;
#endif
        sym->fixup = NULL;
        sym->next = *sym_ptr;
        *sym_ptr = sym;

        sym->name = AsmAlloc( strlen( name ) + 1 );
        strcpy( sym->name, name );
        if( is_current_loc ) {
            return( sym );
        }
        sym->state = AsmQueryExternal( name );
        if( sym->state != SYM_UNDEFINED ) {
            sym->mem_type = CvtTable[ AsmQueryType( name ) ];
        } else {
            sym->mem_type = 0;
        }
    } else {
        AsmError( NO_MEMORY );
    }
    return( sym );
}

static void FreeASym( struct asm_sym *sym )
{
#ifdef _WASM_
    struct asmfixup     *fix;

    for( ;; ) {
        fix = sym->fixup;
        if( fix == NULL ) break;
        sym->fixup = fix->next;
        AsmFree( fix );
    }
#endif
    AsmFree( sym->name );
    AsmFree( sym );
}

#ifdef _WASM_

int AsmChangeName( char *old, char *new )
/***************************************/
{
    struct asm_sym      **sym_ptr;
    struct asm_sym      *sym;

    sym_ptr = AsmFind( old );
    if( *sym_ptr != NULL ) {
        sym = *sym_ptr;
        *sym_ptr = sym->next;
        AsmFree( sym->name );
        sym->name = AsmAlloc( strlen( new ) + 1 );
        strcpy( sym->name, new );

        sym_ptr = AsmFind( new );
        if( *sym_ptr != NULL ) return( ERROR );

        sym->next = *sym_ptr;
        *sym_ptr = sym;
    }
    return( NOT_ERROR );
}

struct asm_sym *AsmAdd( struct asm_sym *sym )
/*******************************************/
{
    struct asm_sym  **location;

    location = AsmFind( sym->name );

    if( *location != NULL ) {
        /* we already have one */
        AsmError( SYMBOL_ALREADY_DEFINED );
        return( NULL );
    }

    sym->offset = 0;
    sym->public = FALSE;
    #ifdef _WASM_
        sym->fixup = NULL;
        sym->mangler = NULL;
    #endif
    sym->next = *location;
    *location = sym;

    sym->state = AsmQueryExternal( sym->name );
    if( sym->state != SYM_UNDEFINED ) {
        sym->mem_type = CvtTable[ AsmQueryType( sym->name ) ];
    } else {
        sym->mem_type = 0;
    }
    return( sym );
}

void AsmTakeOut( char *name )
/***************************/
{
    struct asm_sym      *sym;
    struct asm_sym      **sym_ptr;

    sym_ptr = AsmFind( name );
    if( *sym_ptr != NULL ) {
        /* found it -- so take it out */
        sym = *sym_ptr;
        *sym_ptr = sym->next;
        FreeInfo( (dir_node *)sym );
        FreeASym( sym );
    }
    return;
}

#endif

struct asm_sym *AsmGetSymbol( char *name )
/****************************************/
{
    struct asm_sym  **sym_ptr;

    sym_ptr = AsmFind( name );
    return( *sym_ptr );
}

void AsmSymFini()
{
    struct asm_sym      *sym;
#ifdef _WASM_
    dir_node            *dir;
    unsigned            i;

    FreePubQueue();
    FreeAliasQueue();
    FreeLnameQueue();

    /* now free the symbol table */
    for( i = 0; i < HASH_TABLE_SIZE; i++ ) {
        struct asm_sym  *next;
        next = sym_table[i];
        for( ;; ) {
            sym = next;
            if( sym == NULL ) break;
            dir = (dir_node *)sym;
            next = sym->next;
            FreeInfo( dir );
            FreeASym( sym );
        }
    }

#else
    struct asmfixup     *fix;

    for( ;; ) {
        sym = AsmSymHead;
        if( sym == NULL ) break;
        AsmSymHead = sym->next;
        FreeASym( sym );
    }
    for( ;; ) {
        fix = FixupHead;
        if( fix == NULL ) break;
        FixupHead = fix->next;
        AsmFree( fix );
    }
#endif
}
