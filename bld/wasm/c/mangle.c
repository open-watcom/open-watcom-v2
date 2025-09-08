/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Symbol mangling routines.
*
****************************************************************************/


#include "asmglob.h"
#include "asmalloc.h"
#include "directiv.h"
#include "mangle.h"

#include "clibext.h"


/* constants used by the name manglers ( changes ) */
enum changes {
    USCORE_NONE      = 0,
    USCORE_BEFORE    = 1,
    USCORE_AFTER     = 2
};

#define USCORE "_"

static char *AsmMangler( asm_sym_handle sym )
/*******************************************/
{
    return( AsmStrDup( sym->name ) );
}

static char *UCaseMangler( asm_sym_handle sym )
/*********************************************/
{
    char        *name;

    name = AsmStrDup( sym->name );
    return( strupr( name ) );
}

static char *UScoreMangler( asm_sym_handle sym )
/**********************************************/
{
    char        *name;

    name = AsmAlloc( strlen( sym->name ) + 1 + 1 );
    name[0] = '_';
    strcpy( name + 1, sym->name );
    return( name );
}

static char *StdUScoreMangler( asm_sym_handle sym )
/*************************************************/
{
    if( !Options.mangle_stdcall )
        return( AsmMangler( sym ) );

    if( Options.use_stdcall_at_number
      && ( sym->state == SYM_PROC ) ) {
        int     parasize;
        char    *name;
        int     count;

        parasize = ((dir_node_handle)sym)->e.procinfo->parasize;
        for( count = 2; parasize > 9; count++ )
            parasize /= 10;
        name = AsmAlloc( strlen( sym->name ) + 1 + 1 + count + 1 );
        parasize = ((dir_node_handle)sym)->e.procinfo->parasize;
        sprintf( name, "_%s@%d", sym->name, parasize );
        return( name );
    }
    return( UScoreMangler( sym ) );
}

static char *WatcomCMangler( asm_sym_handle sym )
/***********************************************/
{
    char            *name;
    char            *ptr = sym->name;
    enum changes    changes;
    size_t          len;
    char            *p;

    if( !Options.watcom_params_passed_by_regs
      && SymIs32( sym ) ) {
        changes = USCORE_NONE;
    } else if( sym->state == SYM_PROC ) {
        changes = USCORE_AFTER;
    } else {
        switch( sym->mem_type ) {
        case MT_NEAR:
        case MT_FAR:
        case MT_EMPTY:
        case MT_PROC:
            changes = USCORE_AFTER;
            break;
        default:
            changes = USCORE_BEFORE;
        }
    }

    len = strlen( ptr );
    name = AsmAlloc( len + 2 + 1 );
    p = name;
    if( changes & USCORE_BEFORE ) {
        *p++ = '_';
    }
    p = CATSTR( p, ptr, len );
    if( changes & USCORE_AFTER ) {
        *p++ = '_';
    }
    *p = '\0';
    return( name );
}

static char *CMangler( asm_sym_handle sym )
/*****************************************/
{
    return( UScoreMangler( sym ) );
}

static mangle_func GetMangler( const char *mangle_type )
/******************************************************/
{
    mangle_func mangler;

    mangler = NULL;
    if( mangle_type != NULL ) {
        if( stricmp( mangle_type, "C" ) == 0 ) {
            mangler = WatcomCMangler;
        } else if( stricmp( mangle_type, "N" ) == 0 ) {
            mangler = AsmMangler;
        } else {
            AsmErr( UNKNOWN_MANGLER, mangle_type );
        }
    }
    return( mangler );
}

char *Mangle( asm_sym_handle sym )
/********************************/
{
    mangle_func mangler;

    switch( sym->langtype ) {
    case WASM_LANG_SYSCALL:
        mangler = AsmMangler;
        break;
    case WASM_LANG_STDCALL:
        mangler = StdUScoreMangler;
        break;
    case WASM_LANG_BASIC:
    case WASM_LANG_FORTRAN:
    case WASM_LANG_PASCAL:
        mangler = UCaseMangler;
        break;
    case WASM_LANG_WATCOM_C:
        mangler = WatcomCMangler;
        break;
    case WASM_LANG_C:
        mangler = CMangler;
        break;
    default:
    case WASM_LANG_NONE:
        mangler = sym->mangler;
        if( mangler == NULL )
            mangler = GetMangler( Options.default_name_mangler );
        if( mangler == NULL )
            mangler = AsmMangler;
        break;
    }
    sym->mangler = mangler;
    return( mangler( sym ) );
}

void SetMangler( asm_sym_handle sym, const char *mangle_type, int langtype )
/**************************************************************************/
{
    mangle_func mangler;

    if( langtype != WASM_LANG_NONE )
        sym->langtype = langtype;
    mangler = GetMangler( mangle_type );
    if( mangler == NULL ) {
        /* nothing to do */
    } else if( sym->mangler == NULL ) {
        sym->mangler = mangler;
    } else if( sym->mangler != mangler ) {
        AsmErr( CONFLICTING_MANGLER, sym->name );
    }
}
