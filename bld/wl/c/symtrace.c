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

static trace_info       *CurrTrace;

trace_info       *TraceList;

void ResetSymTrace( void )
/************************/
{
    TraceList = NULL;
}

static void CheckFileTrace( section * );

void CheckTraces( void )
/*****************************/
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
            CurrTrace = info;
            WalkAllSects( CheckFileTrace );
            if( !info->found ) {
                LnkMsg( WRN+MSG_TRACE_OBJ_NOT_FOUND, "s", info->u.name );
                _LnkFree( info->u.name );
            }
            _LnkFree( info );
        } else {
            for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
                if( FNAMECMPSTR( lib->file->name, info->u.name ) == 0 ) {
                    info->found = TRUE;
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

static void CheckFileTrace( section *sect )
/******************************************/
{
    file_list       *list;

    if( CurrTrace->found )
        return;
    for( list = sect->files; list != NULL; list = list->next_file ) {
        if( FNAMECMPSTR( list->file->name, CurrTrace->u.name ) == 0 ) {
            CurrTrace->found = TRUE;
            _LnkFree( CurrTrace->u.name );
            list->status |= STAT_TRACE_SYMS;
            return;
        }
    }
}

void CheckLibTrace( file_list *lib )
/******************************************/
{
    trace_info      *info;

    for( info = TraceList; info != NULL; info = info->next ) {
        if( !info->found ) {
            if( FNAMECMPSTR( info->u.name, lib->file->name ) == 0 ) {
                info->found = TRUE;
                _LnkFree( info->u.name );
                info->u.lib = lib;
                break;
            }
        }
    }
}

bool FindLibTrace( mod_entry *mod )
/****************************************/
{
    trace_info **   prev;
    trace_info      *info;

    prev = &TraceList;
    for( info = TraceList; info != NULL; info = info->next ) {
        if( info->found && info->u.lib == mod->f.source ) {
            if( ModNameCompare( mod->name, info->member ) ) {
                *prev = info->next;
                _LnkFree( info->member );
                _LnkFree( info );
                return( TRUE );
            }
        }
        prev = &info->next;
    }
    return( FALSE );
}

void PrintBadTraces( void )
/********************************/
{
    trace_info      *info;

    for( info = TraceList; info != NULL; info = info->next ) {
        if( info->found ) {
            LnkMsg( WRN+MSG_TRACE_LIB_NOT_FOUND, "12", info->u.lib->file->name,
                                                        info->member );
        } else {
            LnkMsg( WRN+MSG_TRACE_LIB_NOT_FOUND, "12", info->u.name,
                                                        info->member );
        }
    }
    CleanTraces();
}

void CleanTraces( void )
/*****************************/
{
    trace_info *next;

    while( TraceList != NULL ) {
        next = TraceList->next;
        if( !TraceList->found ) {
            _LnkFree( TraceList->u.name );
        }
        _LnkFree( TraceList->member );
        _LnkFree( TraceList );
        TraceList = next;
    }
    TraceList = NULL;
}
