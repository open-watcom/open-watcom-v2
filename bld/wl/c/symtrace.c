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
* Description:  symbol/module tracing routines
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "library.h"
#include "overlays.h"
#include "symtrace.h"


typedef struct trace_info {
    struct trace_info       *next;
    union {
        char                *name;
        file_list           *lib;
    } u;
    char                    *member;
    bool                    found;
} trace_info;

typedef struct sym_trace_info {
    struct sym_trace_info   *next;
    char                    *name;
} sym_trace_info;

static trace_info       *TraceList;
static sym_trace_info   *TraceListSym;

void ResetSymTrace( void )
/************************/
{
    TraceListSym = NULL;
    TraceList = NULL;
}

static void CheckFileTrace( section *sect, void *_info )
/******************************************************/
{
    file_list       *list;
    trace_info      *info = (trace_info *)_info;

    if( info->found )
        return;
    for( list = sect->files; list != NULL; list = list->next_file ) {
        if( FNAMECMPSTR( list->infile->name.u.ptr, info->u.name ) == 0 ) {
            info->found = true;
            _LnkFree( info->u.name );
            list->flags |= STAT_TRACE_SYMS;
            return;
        }
    }
}

void AddTraceListMod( char *fname, char *membname )
/*************************************************/
{
    trace_info      *info;

    _ChkAlloc( info, sizeof( trace_info ) );
    info->u.name = fname;
    info->member = membname;
    info->found = false;        // used for matching libraries
    info->next = TraceList;
    TraceList = info;
}

void CheckTraces( void )
/**********************/
// first check for .obj files being traced, then check libraries
{
    trace_info      *info;
    trace_info      *next;
    file_list       *lib;
    trace_info **   prev;

    prev = &TraceList;
    for( info = TraceList; info != NULL; info = next ) {
        next = info->next;
        if( info->member == NULL ) {
            ParmWalkAllSects( CheckFileTrace, info );
            if( !info->found ) {
                LnkMsg( WRN+MSG_TRACE_OBJ_NOT_FOUND, "s", info->u.name );
                _LnkFree( info->u.name );
            }
            _LnkFree( info );
        } else {
            for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
                if( FNAMECMPSTR( lib->infile->name.u.ptr, info->u.name ) == 0 ) {
                    info->found = true;
                    _LnkFree( info->u.name );
                    info->u.lib = lib;
                    break;
                }
            }
            *prev = info;
            prev = &info->next;
        } /* if */
    } /* for */
    *prev = NULL;
}

void CheckLibTrace( file_list *lib )
/**********************************/
{
    trace_info      *info;

    for( info = TraceList; info != NULL; info = info->next ) {
        if( !info->found ) {
            if( FNAMECMPSTR( info->u.name, lib->infile->name.u.ptr ) == 0 ) {
                info->found = true;
                _LnkFree( info->u.name );
                info->u.lib = lib;
                break;
            }
        }
    }
}

bool FindLibTrace( mod_entry *mod )
/*********************************/
{
    trace_info **   prev;
    trace_info      *info;

    prev = &TraceList;
    for( info = TraceList; info != NULL; info = info->next ) {
        if( info->found && info->u.lib == mod->f.source ) {
            if( ModNameCompare( mod->name.u.ptr, info->member ) ) {
                *prev = info->next;
                _LnkFree( info->member );
                _LnkFree( info );
                return( true );
            }
        }
        prev = &info->next;
    }
    return( false );
}

void AddTraceListSym( char *symname )
/***********************************/
{
    sym_trace_info  *info;

    _ChkAlloc( info, sizeof( sym_trace_info ) );
    info->name = symname;
    info->next = TraceListSym;
    TraceListSym = info;
}

bool FindSymTrace( const char *symname )
/**************************************/
{
    sym_trace_info  *info;
    sym_trace_info  **prev;

    prev = &TraceListSym;
    for( info = TraceListSym; info != NULL; info = info->next ) {
        if( strcmp( info->name, symname ) == 0 ) {
            *prev = info->next;
            _LnkFree( info->name );
            _LnkFree( info );
            return( true );
        }
        prev = &(info->next);
    }
    return( false );
}

void PrintBadTraces( void )
/*************************/
{
    sym_trace_info  *sym_info;
    trace_info      *info;

    for( sym_info = TraceListSym; sym_info != NULL; sym_info = sym_info->next ) {
        LnkMsg( WRN+MSG_TRACE_SYM_NOT_FOUND, "s", sym_info->name );
    }
    for( info = TraceList; info != NULL; info = info->next ) {
        if( info->found ) {
            LnkMsg( WRN+MSG_TRACE_LIB_NOT_FOUND, "12", info->u.lib->infile->name.u.ptr, info->member );
        } else {
            LnkMsg( WRN+MSG_TRACE_LIB_NOT_FOUND, "12", info->u.name, info->member );
        }
    }
    CleanTraces();
}

void CleanTraces( void )
/**********************/
{
    void    *next;

    for( ; TraceListSym != NULL; TraceListSym = next ) {
        next = TraceListSym->next;
        _LnkFree( TraceListSym->name );
        _LnkFree( TraceListSym );
    }
    for( ; TraceList != NULL; TraceList = next ) {
        next = TraceList->next;
        if( !TraceList->found ) {
            _LnkFree( TraceList->u.name );
        }
        _LnkFree( TraceList->member );
        _LnkFree( TraceList );
    }
}
