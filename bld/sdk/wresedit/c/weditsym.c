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


// the following is a temporary measure to get around the fact
// that winreg.h defines a type called ppvalue
#define _WINREG_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "wglbl.h"
#include "wwait.h"
#include "wgetfn.h"
#include "wmem.h"
#include "wmsg.h"
#include "wmsgfile.h"
#include "weditsym.h"
#include "wstrdup.h"
#include "preproc.h"

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static jmp_buf SymEnv;

void PP_OutOfMemory( void )
{
    longjmp( SymEnv, 1 );
}

void *PP_Malloc( unsigned size )
{
    void        *p;

    p = WMemAlloc ( size );
    if( p == NULL ) {
        PP_OutOfMemory();
    }
    return( p );
}

void PP_Free( void *p )
{
    WMemFree ( p );
}

static void addSymbols( WRHashTable *table )
{
    int                 hash;
    MACRO_ENTRY         *me;
    char                *endptr;
    PPVALUE             val;
    WRHashValue         value;
    WRHashEntry         *entry;
    BOOL                dup;

    if( table == NULL ) {
        return;
    }

    dup = TRUE;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( me = PPHashTable[hash]; me; me = me->next ) {
            if( me->parmcount == 0  &&  me->replacement_list != NULL ) {
                if( PPEvalExpr( me->replacement_list, &endptr, &val ) ) {
                    if( *endptr == '\0' ) {
                        if( val.type == PPTYPE_SIGNED ) {
                            value = (WRHashValue) val.val.ivalue;
                        } else {
                            value = (WRHashValue) val.val.uvalue;
                        }
                        entry = WRAddHashEntry( table, me->name, value, &dup, FALSE, FALSE );
                    }
                }
            }
        }
    }
}

char *WLoadSymbols( WRHashTable **table, char *file_name,
                    HWND parent, Bool prompt )
{
    char                *name;
    int                 c;
    unsigned            flags;
    char                *inc_path;
    WGetFileStruct      gf;
    Bool                ret;
    Bool                ok;

    name = NULL;

    ok = ( table != NULL );

    if( ok ) {
        if( !file_name || prompt ) {
            gf.file_name = file_name;
            gf.title     = WAllocRCString( W_LOADSYMTITLE );
            gf.filter    = WAllocRCString( W_SYMFILTER );
            WMassageFilter( gf.filter );
            name = WGetOpenFileName( parent, &gf );
            if( gf.title ) {
                WFreeRCString( gf.title );
            }
            if( gf.filter ) {
                WFreeRCString( gf.filter );
            }
        } else {
            name = WStrDup( file_name );
        }
        ok = ( name != NULL );
    }

    WSetWaitCursor( parent, TRUE );

    if( ok ) {
        flags = 0;
        flags |= PPFLAG_IGNORE_INCLUDE;
        inc_path = NULL;
        ret = setjmp( SymEnv );
        if( ret ) {
            PP_Fini();
            WDisplayErrorMsg( W_SYMOUTOFMEM );
            ok = FALSE;
        }
    }

    if( ok ) {
        ok = !PP_Init( name, flags, inc_path );
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
        PP_Fini();
    }

    if( !ok ) {
        if( name ) {
            WMemFree( name );
            name = NULL;
        }
    }

    WSetWaitCursor( parent, FALSE );

    return( name );
}

Bool WEditSymbols( HWND parent, WRHashTable **symbol_table,
                   HINSTANCE inst, HELP_CALLBACK hcb )
{
    WRHashEntryFlags    flags;
    FARPROC             cb;
    Bool                ret;

    _wtouch(inst);

    if( !symbol_table || !*symbol_table ) {
        return( FALSE );
    }

    flags = WR_HASHENTRY_ALL;
    cb = MakeProcInstance( (FARPROC)hcb, inst );
    ret = WREditSym( parent, symbol_table, &flags, cb );
    FreeProcInstance( (FARPROC)cb );

    return( ret );
}

char *WCreateSymName( char *fname )
{
    char        fn_path[_MAX_PATH];
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];

    if( fname == NULL ) {
        return( NULL );
    }

    _splitpath( fname, fn_drive, fn_dir, fn_name, NULL );
    _makepath( fn_path, fn_drive, fn_dir, fn_name, "h" );

    return( WStrDup( fn_path ) );
}

