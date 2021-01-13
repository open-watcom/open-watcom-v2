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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 *  LIBSUPP   : Library support
 *
 */

#include <string.h>
#include "linkstd.h"
#include "pcobj.h"
#include "msg.h"
#include "objcache.h"
#include "distrib.h"
#include "symtrace.h"
#include "specials.h"
#include "library.h"
#include "procfile.h"
#include "objio.h"

#include "clibext.h"


static bool SearchAndProcLibFile( file_list *lib, const char *name )
/******************************************************************/
{
    mod_entry       *lp;
    mod_entry       **prev;

    if( !CacheOpen( lib ) )
        return( false );
    lp = SearchLib( lib, name );
    if( lp == NULL ) {
        CacheClose( lib, 1 );
        return( false );
    }
    lib->flags |= STAT_LIB_USED;
#ifdef _EXE
    if( (FmtData.type & MK_OVERLAYS) && FmtData.u.dos.distribute ) {
        if( lib->flags & STAT_LIB_FIXED ) {
            lp->modinfo |= MOD_FIXED;
        }
        DistribAddMod( lp, lib->ovlref );
    } else {
#endif
        for( prev = &LibModules; *prev != NULL; ) { /*  find end of list */
            prev = &(*prev)->n.next_mod;
        }
        *prev = lp;
#ifdef _EXE
    }
#endif
    CurrMod = lp;
    ObjPass1();
    CacheClose( lib, 1 );
#ifdef _EXE
    if( (FmtData.type & MK_OVERLAYS) && FmtData.u.dos.distribute ) {
        DistribFinishMod( lp );
    }
#endif
    if( FindLibTrace( lp ) ) {
        TraceSymList( lp->publist );
    }
    return( true );
}

#define PREFIX_LEN (sizeof( ImportSymPrefix ) - 1)

bool LibFind( const char *name, bool old_sym )
/*********************************************/
/* Search for a file in a library */
{
    file_list   *lib;
    bool        isimpsym;

    DEBUG(( DBG_OLD, "LibFind( %s )", name ));
    isimpsym = (FmtData.type & MK_PE) && memcmp( name, ImportSymPrefix, PREFIX_LEN ) == 0;
    for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
        if( lib->infile->status & INSTAT_IOERR )
            continue;
        if( old_sym && (lib->flags & STAT_OLD_LIB) )
            continue;
        if( SearchAndProcLibFile( lib, name ) )
            return( true );
        if( isimpsym ) {
            if( SearchAndProcLibFile( lib, name + PREFIX_LEN ) ) {
                return( true );
            }
        }
    }
    return( false );
}

bool ModNameCompare( const char *tname, const char *membname )
/************************************************************/
// check if a THEADR record name is equal to a library member name
{
    size_t      lentheadr;
    size_t      lenmember;
    const char  *namestart;

    namestart = GetBaseName( tname, 0, &lentheadr );
    lenmember = strlen( membname );
    if( lentheadr == lenmember ) {
        if( strnicmp( namestart, membname, lenmember ) == 0 ) {
            return( true );
        }
    }
    return( false );
}

file_list *AddObjLib( const char *name, lib_priority priority )
/*************************************************************/
{
    file_list   **owner;
    file_list   **new_owner;
    file_list   *lib;

    DEBUG(( DBG_OLD, "Adding Object library name %s", name ));
    /* search for new library position in linked list */
    for( owner = &ObjLibFiles; (lib = *owner) != NULL; owner = &lib->next_file ) {
        if( lib->priority < priority )
            break;
        /* end search if library already exists with same or a higher priority */
        if( FNAMECMPSTR( lib->infile->name.u.ptr, name ) == 0 ) {
            return( lib );
        }
    }
    new_owner = owner;
    /* search for library definition with a lower priority */
    for( ; (lib = *owner) != NULL; owner = &lib->next_file ) {
        if( FNAMECMPSTR( lib->infile->name.u.ptr, name ) == 0 ) {
            /* remove library entry from linked list */
            *owner = lib->next_file;
            break;
        }
    }
    /* if we need to add one */
    if( lib == NULL ) {
        lib = AllocNewFile( NULL );
        lib->infile = AllocUniqueFileEntry( name, UsrLibPath );
        lib->infile->status |= INSTAT_LIBRARY | INSTAT_OPEN_WARNING;
        LinkState |= LS_LIBRARIES_ADDED;
    }
    /* put it to new position and setup priority */
    lib->next_file = *new_owner;
    *new_owner = lib;
    lib->priority = priority;

    return( lib );
}
