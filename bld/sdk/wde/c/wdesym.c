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

#include "wdeglbl.h"
#include "wdemem.h"
#include "wderesin.h"
#include "wdetfile.h"
#include "wdegetfn.h"
#include "wdeopts.h"
#include "wdemain.h"
#include "wdegeted.h"
#include "wdestat.h"
#include "wdesdup.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdefutil.h"
#include "wdewait.h"
#include "wdestken.h"
#include "wdedebug.h"
#include "wde_rc.h"
#include "wdectl3d.h"
#include "wdecurr.h"
#include "wdesym.h"
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
static Bool WdeResourceViewHash( WdeResInfo * );
static Bool WdeResourceLoadHash( WdeResInfo * );
static Bool WdeResourceWriteHash( WdeResInfo * );
static void WdeAddSymbols( WdeHashTable * );
static char *WdeLoadSymbols( WdeHashTable **, char *, Bool );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern char     *WdeSymSaveFilter;
extern char     *WdeLoadHeaderTitle;
extern char     *WdeWriteHeaderTitle;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char  WdeBusyChars[]         = "-\\|/";

static Bool WdeViewSymbols( WdeHashTable **table, HWND parent )
{
    WRHashEntryFlags    flags;
    FARPROC             cb;
    Bool                ok;

    cb = NULL;
    ok = (table != NULL);

    if( ok ) {
        cb = MakeProcInstance( (FARPROC)WdeHelpRoutine, WdeGetAppInstance() );
        ok = (cb != (FARPROC)NULL);
    }

    if( ok ) {
        flags = WR_HASHENTRY_ALL;
        ok = WREditSym( parent, table, &flags, cb );
    }

    if( cb != (FARPROC)NULL ) {
        FreeProcInstance( (FARPROC)cb );
    }

    return( ok );
}

Bool WdeResourceHashTableAction( WdeResInfo *info, int action )
{
    Bool ret;

    ret = FALSE;

    switch( action ) {
    case VIEW_HASH:
        ret = WdeResourceViewHash( info );
        break;
    case LOAD_HASH:
        ret = WdeResourceLoadHash( info );
        break;
    case WRITE_HASH:
        ret = WdeResourceWriteHash( info );
        break;
    }

    return( ret );
}

Bool WdeResourceViewHash( WdeResInfo *info )
{
    BOOL    no_hash;
    BOOL    ret;
    OBJPTR  obj;

    if( info->hash_table == NULL ) {
        InitState( info->forms_win );
        no_hash = TRUE;
        info->hash_table = WdeInitHashTable();
    }

    ret = WdeViewSymbols( &info->hash_table, info->edit_win );

    if( !WdeNumInHashTable( info->hash_table ) ) {
        WdeFreeHashTable( info->hash_table );
        info->hash_table = NULL;
    }

    if( ret && (obj = GetMainObject()) != NULL ) {
        WdeTouchHashTable( info->hash_table );
        Forward( obj, RESOLVE_HELPSYMBOL, &ret, NULL ); /* JPK */
        Forward( obj, RESOLVE_SYMBOL, &ret, NULL );
    }

    if( ret ) {
        obj = GetCurrObject();
        if( obj ) {
            Notify( obj, PRIMARY_OBJECT, NULL );
        }
    }

    return( ret );
}

Bool WdeResourceLoadHash( WdeResInfo *info )
{
    OBJPTR      obj;
    Bool        b;
    Bool        from_id;
    char        *include;

    include = WdeLoadSymbols( &info->hash_table, NULL, TRUE );
    if( include == NULL ) {
        return( FALSE );
    }

    if( info->sym_name != NULL ) {
        WdeMemFree( info->sym_name );
    }
    info->sym_name = include;

    if( (obj = GetMainObject()) != NULL ) {
        b = TRUE;
        from_id = TRUE;
        Forward( obj, RESOLVE_HELPSYMBOL, &b, &from_id ); /* JPK */
        Forward( obj, RESOLVE_SYMBOL, &b, &from_id );
    }

    obj = GetCurrObject();
    if( obj ) {
        Notify( obj, PRIMARY_OBJECT, NULL );
    }

    WdeSetResModified( info, TRUE );

    return( TRUE );
}

Bool WdeResourceWriteHash( WdeResInfo *info )
{
    return( WdeWriteSymbols( info->hash_table, &info->sym_name, TRUE ) );
}

Bool WdeCreateDLGInclude( WdeResInfo *rinfo, char *include )
{
    WResID              *type;
    WResID              *res;
    WResLangType        lang;
    char                *str;
    int                 len;
    Bool                ok;

    type = NULL;
    res = NULL;
    str = NULL;
    ok = (rinfo != NULL && include != NULL);

    if( ok ) {
        if( rinfo->info->dir == NULL ) {
            rinfo->info->dir = WResInitDir();
            ok = (rinfo->info->dir != NULL);
        }
    }

    if( ok ) {
        WdeDeleteDLGInclude( rinfo );
        type = WResIDFromNum( (uint_16)RT_RCDATA );
        ok = (type != NULL);
    }

    if( ok ) {
        res = WResIDFromStr( "DLGINCLUDE" );
        ok = (res != NULL);
    }

    if( ok ) {
        str = WdeStrDup( include );
        ok = (str != NULL);
    }

    if( ok ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        len = strlen( include ) + 1;
        ok = !WResAddResource( type, res, MEMFLAG_DISCARDABLE,
                               0, len, rinfo->info->dir, &lang, NULL );
    }

    if( ok ) {
        ok = WRFindAndSetData( rinfo->info->dir, type, res, &lang, str );
    }

    if( type != NULL ) {
        WdeMemFree( type );
    }

    if( res != NULL ) {
        WdeMemFree( res );
    }

    if( !ok ) {
        if( str != NULL ) {
            WdeMemFree( str );
        }
    }

    return( ok );
}

Bool WdeDeleteDLGInclude( WdeResInfo *rinfo )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    Bool                ok;

    ok = (rinfo != NULL);

    if( ok ) {
        tnode = WRFindTypeNode( rinfo->info->dir, (uint_16)RT_RCDATA, NULL );
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
        ok = WRRemoveLangNodeFromDir( rinfo->info->dir, &tnode, &rnode, &lnode );
    }

    return( ok );
}

static char *WdeFindDLGInclude( WdeResInfo *rinfo )
{
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WResLangType        lang;
    char                *include;
    Bool                ok;

    include = NULL;
    ok = (rinfo != NULL);

    if( ok ) {
        tnode = WRFindTypeNode( rinfo->info->dir, (uint_16)RT_RCDATA, NULL );
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
        include = (char *)WRCopyResData( rinfo->info, lnode );
    }

    return( include );
}

char *WdeCreateSymName( char *fname )
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

    return( WdeStrDup( fn_path ) );
}

Bool WdeFindAndLoadSymbols( WdeResInfo *rinfo )
{
    char        fn_path[_MAX_PATH];
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];
    char        *include;
    Bool        prompt;
    Bool        ret;

    include = NULL;

    if( rinfo == NULL || rinfo->info->file_name == NULL ) {
        return( FALSE );
    }

    include = WdeFindDLGInclude( rinfo );
    if( include != NULL && !WdeFileExists( include ) ) {
        WdeMemFree( include );
        include = NULL;
    }

    if( include == NULL ) {
        _splitpath( rinfo->info->file_name, fn_drive, fn_dir, fn_name, NULL );
        _makepath( fn_path, fn_drive, fn_dir, fn_name, "h" );
        prompt = TRUE;
    } else {
        strcpy( fn_path, include );
        WdeMemFree( include );
        include = NULL;
        prompt = FALSE;
    }

    ret = TRUE;

    if( WdeFileExists( fn_path ) ) {
        include = WdeLoadSymbols( &rinfo->hash_table, fn_path, prompt );
        ret = (include != NULL);
        if( ret ) {
            if( rinfo->sym_name != NULL ) {
                WdeMemFree( rinfo->sym_name );
            }
            rinfo->sym_name = include;
            WdeSetResModified( rinfo, TRUE );
        }
    }

    return( ret );
}

static jmp_buf SymEnv;

char *WdeLoadSymbols( WdeHashTable **table, char *file_name, Bool prompt )
{
    char                *name;
    int                 c;
    unsigned            flags;
    char                *inc_path;
    WdeGetFileStruct    gf;
    Bool                ret;
    Bool                ok;
    Bool                pop_env;
    unsigned            pp_count;
    unsigned            busy_count;
    char                busy_str[2];

    pop_env = FALSE;
    name = NULL;

    ok = (table != NULL);

    if( ok ) {
        WdeSetStatusText( NULL, " ", FALSE );
        WdeSetStatusByID( WDE_LOADINGSYMBOLS, -1 );
    }

    if( ok ) {
        if( file_name == NULL || prompt ) {
            gf.file_name = file_name;
            gf.title = WdeLoadHeaderTitle;
            gf.filter = WdeSymSaveFilter;
            name = WdeGetOpenFileName( &gf );
        } else {
            name = WdeStrDup( file_name );
        }
        ok = (name != NULL);
    }

    WdeSetWaitCursor( TRUE );

    if( ok ) {
        flags = PPFLAG_EMIT_LINE;
        if( WdeGetOption( WdeOptIgnoreInc ) ) {
            flags |= PPFLAG_IGNORE_INCLUDE;
        }
        inc_path = WdeGetIncPathOption();

        ret = setjmp( SymEnv );
        if( ret ) {
            ok = FALSE;
            PP_Fini();
        } else {
            ok = (pop_env = WdePushEnv( &SymEnv ));
        }
    }

    if( ok ) {
        ok = !PP_Init( name, flags, inc_path );
        if( !ok ) {
            WdeWriteTrail( "WdeLoadSymbols: Unable to open header file!" );
            WdeDisplayErrorMsg( WDE_NOLOADHEADERFILE );
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
                busy_str[0] = WdeBusyChars[busy_count % 4];
                WdeSetStatusText( NULL, busy_str, TRUE );
                pp_count = 0;
            }
        } while( c != EOF );
        if( *table == NULL ) {
            *table = WdeInitHashTable();
        }
        WdeAddSymbols( *table );
        WdeMakeHashTableClean( *table );
        WdeSetStatusText( NULL, " ", TRUE );
        PP_Fini();
    }

    if( pop_env ) {
        WdePopEnv( &SymEnv );
    }

    if( !ok ) {
        if( name != NULL ) {
            WdeMemFree( name );
            name = NULL;
        }
    }

    WdeSetWaitCursor( FALSE );

    WdeSetStatusReadyText();

    return( name );
}

Bool WdeWriteSymbols( WdeHashTable *table, char **file_name, Bool prompt )
{
    char                *name;
    WdeGetFileStruct    gf;

    if( table == NULL || file_name == NULL ) {
        return( FALSE );
    }

    if( WRIsDefaultHashTable( table ) ) {
        return( TRUE );
    }

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_WRITINGSYMBOLS, -1 );

    if( prompt || *file_name == '\0' ) {
        gf.file_name = *file_name;
        gf.title = WdeWriteHeaderTitle;
        gf.filter = WdeSymSaveFilter;
        name = WdeGetSaveFileName( &gf );
        if( name == NULL ) {
            return( FALSE );
        }
        if( *file_name != NULL ) {
            WdeMemFree( *file_name );
        }
        *file_name = name;
    } else {
        name = *file_name;
    }

    if( WdeWriteSymbolsToFile( table, name ) ) {
        WdeMakeHashTableClean( table );
    }

    WdeSetStatusReadyText();

    return( TRUE );
}

void WdeAddSymbols( WdeHashTable *table )
{
    int                 hash;
    MACRO_ENTRY         *me;
    char                *endptr;
    PREPROC_VALUE       val;
    WdeHashValue        value;
    void                *vp;
    Bool                dup;
    unsigned            add_count;
    unsigned            busy_count;
    char                busy_str[2];

    if( table == NULL ) {
        WdeWriteTrail( "WdeAddSymbols: unexpected NULL hash table.");
        return;
    }

    dup = TRUE;
    add_count = 0;
    busy_count = 0;
    busy_str[1] = '\0';

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( me = PPHashTable[hash]; me; me = me->next ) {
            if( me->parmcount == 0 && me->replacement_list != NULL ) {
                if( PPEvalExpr( me->replacement_list, &endptr, &val ) ) {
                    if( *endptr == '\0' ) {
                        if( val.type == PPTYPE_SIGNED ) {
                            value = (WdeHashValue)val.val.ivalue;
                        } else {
                            value = (WdeHashValue)val.val.uvalue;
                        }
                        vp = (void *)WdeAddHashEntry( table, me->name, value, &dup );
                        add_count++;
                        if( add_count == MAX_SYM_ADDS ) {
                            busy_count++;
                            busy_str[0] = WdeBusyChars[busy_count % 4];
                            WdeSetStatusText( NULL, busy_str, TRUE );
                            add_count = 0;
                        }
                    }
                }
            }
        }
    }
}
