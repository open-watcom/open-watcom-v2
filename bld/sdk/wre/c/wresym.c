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


#include "precomp.h"
#include "win1632.h"
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "wreglbl.h"
#include "wremain.h"
#include "wregetfn.h"
#include "wrestat.h"
#include "wreopts.h"
#include "wremsg.h"
#include "wremem.h"
#include "wregetfn.h"
#include "wreresin.h"
#include "wrestrdp.h"
#include "wrewait.h"
#include "wrtmpfil.h"
#include "rcstr.gh"
#include "preproc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_PP_CHARS    512
#define MAX_SYM_ADDS    128

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern char *WRESymSaveFilter;
extern char *WRESymLoadTitle;
extern char *WRESymSaveTitle;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char WREBusyChars[] = "-\\|/";
static jmp_buf SymEnv;

void PP_OutOfMemory( void )
{
    longjmp( SymEnv, 1 );
}

void *PP_Malloc( unsigned size )
{
    void        *p;

    p = WREMemAlloc( size );
    if( p == NULL ) {
        PP_OutOfMemory();
    }
    return( p );
}

void PP_Free( void *p )
{
    WREMemFree( p );
}

static char *WREFindDLGInclude( WRInfo *info )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    char                *include;
    Bool                ok;

    include = NULL;
    ok = (info != NULL);

    if( ok ) {
        tnode = WRFindTypeNode( info->dir, (uint_16)RT_RCDATA, NULL );
        ok = (tnode != NULL);
    }

    if( ok ) {
        rnode = WRFindResNode( tnode, 0, "DLGINCLUDE" );
        ok = (rnode != NULL);
    }

    if( ok ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        lnode = WRFindLangNodeFromLangType( rnode, &lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        include = (char *)WRCopyResData( info, lnode );
    }

    return( include );
}

static void WREAddSymbols( WRHashTable *table )
{
    int                 hash;
    MACRO_ENTRY         *me;
    char                *endptr;
    PREPROC_VALUE       val;
    WRHashValue         value;
    WRHashEntry         *entry;
    BOOL                dup;
    unsigned            add_count;
    unsigned            busy_count;
    char                busy_str[2];

    if( table == NULL ) {
        return;
    }

    dup = TRUE;
    add_count = 0;
    busy_count = 0;
    busy_str[1] = '\0';

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( me = PPHashTable[hash]; me; me = me->next ) {
            if( me->parmcount == 0 &&  me->replacement_list != NULL ) {
                if( PPEvalExpr( me->replacement_list, &endptr, &val ) ) {
                    if( *endptr == '\0' ) {
                        if( val.type == PPTYPE_SIGNED ) {
                            value = (WRHashValue)val.val.ivalue;
                        } else {
                            value = (WRHashValue)val.val.uvalue;
                        }
                        entry = WRAddHashEntry( table, me->name, value, &dup, FALSE, FALSE );
                        add_count++;
                        if( add_count == MAX_SYM_ADDS ) {
                            busy_count++;
                            busy_str[0] = WREBusyChars[busy_count % 4];
                            WRESetStatusText( NULL, busy_str, TRUE );
                            add_count = 0;
                        }
                    }
                }
            }
        }
    }
}

static char *WRELoadSymbols( WRHashTable **table, char *file_name, Bool prompt )
{
    char                *name;
    int                 c;
    unsigned            flags;
    char                *inc_path;
    WREGetFileStruct    gf;
    unsigned            pp_count;
    unsigned            busy_count;
    char                busy_str[2];
    Bool                ret;
    Bool                ok;

    name = NULL;

    ok = (table != NULL);

    if( ok ) {
        WRESetStatusText( NULL, "", FALSE );
        WRESetStatusByID( WRE_LOADINGSYMBOLS, -1 );
    }

    if( ok ) {
        if( file_name == NULL || prompt ) {
            gf.file_name = file_name;
            gf.title = WRESymLoadTitle;
            gf.filter = WRESymSaveFilter;
            gf.save_ext = FALSE;
            name = WREGetOpenFileName( &gf );
        } else {
            name = WREStrDup( file_name );
        }
        ok = (name != NULL);
    }

    WRESetWaitCursor( TRUE );

    if( ok ) {
        flags = PPFLAG_IGNORE_INCLUDE | PPFLAG_EMIT_LINE;
        inc_path = NULL;
        ret = setjmp( SymEnv );
        if( ret ) {
            PP_Fini();
            WREDisplayErrorMsg( WRE_SYMOUTOFMEM );
            ok = FALSE;
        }
    }

    if( ok ) {
        ok = !PP_Init( name, flags, inc_path );
        if( !ok ) {
            WREDisplayErrorMsg( WRE_NOLOADHEADERFILE );
        }
    }

    if( ok ) {
        pp_count = 0;
        busy_count = 0;
        busy_str[1] = '\0';
        do {
            pp_count++;
            c = PP_Char();
            if( pp_count == MAX_PP_CHARS ) {
                busy_count++;
                busy_str[0] = WREBusyChars[busy_count % 4];
                WRESetStatusText( NULL, busy_str, TRUE );
                pp_count = 0;
            }
        } while( c != EOF );
        if( *table == NULL ) {
            *table = WRInitHashTable();
        }
        WREAddSymbols( *table );
        WRMakeHashTableClean( *table );
        PP_Fini();
        WRESetStatusText( NULL, " ", TRUE );
    }

    if( !ok ) {
        if( name != NULL ) {
            WREMemFree( name );
            name = NULL;
        }
    }

    WRESetWaitCursor( FALSE );

    WRESetStatusReadyText();

    return( name );
}

Bool WRESaveSymbols( WRHashTable *table, char **file_name, Bool prompt )
{
    char                *name;
    WREGetFileStruct    gf;
    Bool                ok;

    if( table == NULL || file_name == NULL ) {
        return( FALSE );
    }

    if( WRIsDefaultHashTable( table ) ) {
        return( TRUE );
    }

    ok = TRUE;
    WRESetStatusText( NULL, "", FALSE );
    WRESetStatusByID( WRE_SAVEINGSYMBOLS, -1 );

    if( prompt || *file_name == NULL ) {
        gf.file_name = *file_name;
        gf.title = WRESymSaveTitle;
        gf.filter = WRESymSaveFilter;
        gf.save_ext = FALSE;
        name = WREGetSaveFileName( &gf );
        ok = (name != NULL);
        if( ok ) {
            if( *file_name != NULL ) {
                WREMemFree( *file_name );
            }
            *file_name = name;
        }
    } else {
        name = *file_name;
    }

    if( ok ) {
        ok = WRWriteSymbolsToFile( table, name );
    }

    if( ok ) {
        WRMakeHashTableClean( table );
    }

    WRESetStatusReadyText();

    return( ok );
}

Bool WREEditResourceSymbols( WREResInfo *info )
{
    WRHashEntryFlags    flags;
    FARPROC             cb;
    Bool                ok;

    cb = NULL;
    ok = (info != NULL && info->symbol_table != NULL);

    if( ok ) {
        cb = MakeProcInstance( (FARPROC)WREHelpRoutine, WREGetAppInstance() );
        ok = (cb != (FARPROC)NULL);
    }

    if( ok ) {
        flags = WR_HASHENTRY_ALL;
        ok = WREditSym( info->info_win, &info->symbol_table, &flags, cb );
    }

    // ***** call routine to update the edit sessions *****

    if( cb != (FARPROC)NULL ) {
        FreeProcInstance( (FARPROC)cb );
    }

    return( ok );
}

Bool WRELoadResourceSymbols( WREResInfo *info )
{
    char        *symbol_file;

    if( info == NULL ) {
        return( FALSE );
    }

    symbol_file = WRELoadSymbols( &info->symbol_table, NULL, TRUE );
    if( symbol_file == NULL ) {
        return( FALSE );
    }

    if( info->symbol_file != NULL ) {
        WREMemFree( info->symbol_file );
    }
    info->symbol_file = symbol_file;

    WRESetResModified( info, TRUE );

    // ***** call routine to update the edit sessions *****

    return( TRUE );
}

Bool WREResourceSaveSymbols( WREResInfo *info )
{
    if( info != NULL ) {
        return( WRESaveSymbols( info->symbol_table, &info->symbol_file, TRUE ) );
    }
    return( FALSE );
}

Bool WREDeleteDLGInclude( WResDir dir )
{
    return( WRDeleteDLGInclude( dir ) );
}

Bool WRECreateDLGInclude( WResDir *dir, char *include )
{
    return( WRCreateDLGInclude( dir, include ) );
}

char *WRECreateSymName( char *fname )
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

    return( WREStrDup( fn_path ) );
}

Bool WREFindAndLoadSymbols( WREResInfo *rinfo )
{
    char        inc_path[_MAX_PATH];
    char        fn_path[_MAX_PATH];
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];
    char        *symbol_file;
    Bool        prompt;
    Bool        ret;

    if( rinfo == NULL || rinfo->info == NULL ||
        (rinfo->info->file_name == NULL && rinfo->info->save_name == NULL) ) {
        return( FALSE );
    }

    symbol_file = WREFindDLGInclude( rinfo->info );
    if( symbol_file == NULL ) {
        if( rinfo->info->file_name ) {
            _splitpath( rinfo->info->file_name, fn_drive, fn_dir, fn_name, NULL );
        } else {
            _splitpath( rinfo->info->save_name, fn_drive, fn_dir, fn_name, NULL );
        }
        _makepath( fn_path, fn_drive, fn_dir, fn_name, "h" );
        _makepath( inc_path, fn_drive, fn_dir, "", "" );
        WRESetInitialDir( inc_path );
        prompt = TRUE;
    } else {
        strcpy( fn_path, symbol_file );
        WREMemFree( symbol_file );
        symbol_file = NULL;
        prompt = FALSE;
    }

    ret = TRUE;

    if( WRFileExists( fn_path ) ) {
        symbol_file = WRELoadSymbols( &rinfo->symbol_table, fn_path, prompt );
        ret = (symbol_file != NULL);
        if( ret ) {
            if( rinfo->symbol_file != NULL ) {
                WREMemFree( rinfo->symbol_file );
            }
            rinfo->symbol_file = symbol_file;
        }
    }

    return( ret );
}
