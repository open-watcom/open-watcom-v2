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


#include "wglbl.h"
#include <setjmp.h>
#include "wrdll.h"
#include "wwait.h"
#include "wgetfn.h"
#include "wmsg.h"
#include "ldstr.h"
#include "rcstr.grh"
#include "weditsym.h"
#include "wstrdup.h"
#include "preproc.h"
#include "pathgrp2.h"

#include "clibext.h"


typedef struct {
    WRHashTable    *table;
    bool            dup;
} addsym_data;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static jmp_buf SymEnv;

void PPENTRY PP_OutOfMemory( void )
{
    longjmp( SymEnv, 1 );
}

void * PPENTRY PP_Malloc( size_t size )
{
    void        *p;

    p = WRMemAlloc( size );
    if( p == NULL ) {
        PP_OutOfMemory();
    }
    return( p );
}

void PPENTRY PP_Free( void *p )
{
    WRMemFree( p );
}

static void addsym_func( const MACRO_ENTRY *me, const PREPROC_VALUE *val, void *cookie )
{
    WRHashValue         value;

    if( val->type == PPTYPE_SIGNED ) {
        value = (WRHashValue)val->val.ivalue;
    } else {
        value = (WRHashValue)val->val.uvalue;
    }
    WRAddHashEntry( ((addsym_data *)cookie)->table, me->name, value, &(((addsym_data *)cookie)->dup), false, false );
}

static void addSymbols( WRHashTable *table )
{
    addsym_data         data;

    if( table == NULL ) {
        return;
    }
    data.dup = true;
    data.table = table;

    PP_MacrosWalk( addsym_func, &data );
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    /* unused parameters */ (void)p;

    return( 1 );
}

char *WLoadSymbols( WRHashTable **table, char *file_name, HWND parent, bool prompt )
{
    char                *name;
    int                 c;
    pp_flags            ppflags;
    char                *inc_path;
    WGetFileStruct      gf;
    bool                ret;
    bool                ok;

    name = NULL;

    PP_Init( '#' );

    ok = (table != NULL);

    if( ok ) {
        if( file_name == NULL || prompt ) {
            gf.file_name = file_name;
            gf.title = AllocRCString( W_LOADSYMTITLE );
            gf.filter = AllocRCString( W_SYMFILTER );
            WMassageFilter( gf.filter );
            name = WGetOpenFileName( parent, &gf );
            if( gf.title != NULL ) {
                FreeRCString( gf.title );
            }
            if( gf.filter != NULL ) {
                FreeRCString( gf.filter );
            }
        } else {
            name = WStrDup( file_name );
        }
        ok = (name != NULL);
    }

    WSetWaitCursor( parent, true );

    if( ok ) {
        ppflags = PPFLAG_IGNORE_INCLUDE | PPFLAG_EMIT_LINE | PPFLAG_TRUNCATE_FILE_NAME;
        inc_path = NULL;
        ret = setjmp( SymEnv ) != 0;
        if( ret ) {
            PP_FileFini();
            WDisplayErrorMsg( W_SYMOUTOFMEM );
            ok = false;
        }
    }

    if( ok ) {
        ok = !PP_FileInit( name, ppflags, inc_path );
        if( !ok ) {
            WDisplayErrorMsg( W_NOOPENSYMFILE );
        }
    }

    if( ok ) {
        do {
            c = PP_Char();
        } while( c != EOF );
        if( *table == NULL ) {
            *table = WRInitHashTable();
        }
        addSymbols( *table );
        WRMakeHashTableClean( *table );
        PP_FileFini();
    }

    if( !ok ) {
        if( name != NULL ) {
            WRMemFree( name );
            name = NULL;
        }
    }

    PP_Fini();

    WSetWaitCursor( parent, false );

    return( name );
}

bool WEditSymbols( HWND parent, WRHashTable **symbol_table,
                   HINSTANCE inst, HELPFUNC help_callback )
{
    WRHashEntryFlags    flags;
    HELPFUNC            hcb;
    bool                ret;

    /* unused parameters */ (void)inst;

    if( symbol_table == NULL || *symbol_table == NULL ) {
        return( FALSE );
    }

    flags = WR_HASHENTRY_ALL;

    hcb = MakeProcInstance_HELP( help_callback, inst );
    ret = WREditSym( parent, symbol_table, &flags, hcb );
    FreeProcInstance_HELP( hcb );

    return( ret );
}

char *WCreateSymFileName( const char *fname )
{
    pgroup2     pg;
    char        fn_path[_MAX_PATH];

    if( fname == NULL ) {
        return( NULL );
    }

    _splitpath2( fname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    _makepath( fn_path, pg.drive, pg.dir, pg.fname, "h" );

    return( WStrDup( fn_path ) );
}
