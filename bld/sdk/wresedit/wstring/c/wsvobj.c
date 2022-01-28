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
#include "wwait.h"
#include "wrdll.h"
#include "wselft.h"
#include "wgetfn.h"
#include "winst.h"
#include "wmsg.h"
#include "wmain.h"
#include "weditsym.h"
#include "wsvobj.h"
#include "ldstr.h"
#include "sysall.rh"
#include "wstr2rc.h"
#include "wresdefn.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool             WSaveObjectAs( bool, WStringEditInfo *, WRSaveIntoData * );
static bool             WSaveObjectInto( WStringEditInfo *, WRSaveIntoData * );
static WRSaveIntoData   *WMakeSaveData( WStringTable *tbl );
static void             WFreeSaveIntoData( WRSaveIntoData *idata );
static WRSaveIntoData   *WAllocSaveIntoData( void );
static WRSaveIntoData   *WInitSaveData( WStringBlock *, WResID *, WResLangType * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static bool WSaveObjectToRC( WStringEditInfo *einfo, const char *filename,
                             bool shadow, bool append )
{
    pgroup2     pg;
    char        fn_path[_MAX_PATH];

    if( einfo == NULL || filename == NULL ) {
        return( false );
    }

    if( shadow ) {
        _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( fn_path, pg.drive, pg.dir, pg.fname, "str" );
    } else {
        strcpy( fn_path, filename );
    }

    if( WRFileExists( fn_path ) ) {
        WRBackupFile( fn_path, true );
    }

    if( !WWriteStringToRC( einfo, fn_path, append ) ) {
        return( false );
    }

    return( true );
}

bool WSaveObject( WStringEditInfo *einfo, bool get_name, bool save_into )
{
    WRSaveIntoData      *idata;
    bool                ok;

    idata = NULL;

    WSetWaitCursor( einfo->win, true );

    ok = (einfo != NULL && einfo->tbl != NULL);

    if( ok ) {
        if( !WRIsDefaultHashTable( einfo->info->symbol_table ) &&
            (get_name || WRIsHashTableDirty( einfo->info->symbol_table )) ) {
            if( einfo->info->symbol_file == NULL ) {
                char    *fname;
                if( einfo->file_name == NULL ) {
                    fname = einfo->info->file_name;
                } else {
                    fname = einfo->file_name;
                }
                einfo->info->symbol_file = WCreateSymFileName( fname );
            }
            ok = WSaveSymbols( einfo, einfo->info->symbol_table,
                               &einfo->info->symbol_file, get_name );
        }
    }

    if( ok ) {
        idata = WMakeSaveData( einfo->tbl );
        ok = (idata != NULL);
        if( !ok ) {
            WDisplayErrorMsg( W_STRINGTABLEISEMPTY );
        }
    }

    if( ok ) {
        if( save_into ) {
            ok = WSaveObjectInto( einfo, idata );
        } else {
            ok = WSaveObjectAs( get_name, einfo, idata );
        }
    }

    if( ok && einfo->info->stand_alone ) {
        einfo->info->modified = false;
    }

    if( idata != NULL ) {
        WFreeSaveIntoData( idata );
    }

    WSetWaitCursor( einfo->win, false );

    return( ok );
}

bool WSaveObjectAs( bool get_name, WStringEditInfo *einfo, WRSaveIntoData *idata )
{
    char                resfile[_MAX_PATH];
    char                *fname;
    WRFileType          ftype;
    WRFileType          rtype;
    WGetFileStruct      gf;
    WRSaveIntoData      idata2;
    WResLangType        lang;
    bool                got_name;
    bool                ok;

    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    fname = NULL;
    got_name = false;

    ok = (einfo != NULL);

    if( ok ) {
        memset( &idata2, 0, sizeof( idata2 ) );
        if( einfo->info->symbol_file != NULL ) {
            idata2.next = NULL;
            idata2.type = WResIDFromNum( RESOURCE2INT( RT_RCDATA ) );
            idata2.name = WResIDFromStr( "DLGINCLUDE" );
            idata2.data = einfo->info->symbol_file;
            idata2.lang = lang;
            idata2.size = (uint_32)( strlen( einfo->info->symbol_file ) + 1 );
            idata2.MemFlags = MEMFLAG_DISCARDABLE;
        }
    }

    if( ok ) {
        if( einfo->file_name == NULL || get_name ) {
            gf.file_name = NULL;
            gf.title = AllocRCString( W_SAVERESAS );
            gf.filter = AllocRCString( W_SAVERESFILTER );
            WMassageFilter( gf.filter );
            fname = WGetSaveFileName( einfo->win, &gf );
            if( gf.title != NULL ) {
                FreeRCString( gf.title );
            }
            if( gf.filter != NULL ) {
                FreeRCString( gf.filter );
            }
            if( fname != NULL ) {
                got_name = true;
            }
        } else {
            fname = einfo->file_name;
        }
        ok = (fname != NULL);
    }

    if( ok ) {
        if( got_name ) {
            ftype = WSelectFileType( einfo->win, fname, einfo->info->is32bit, true,
                                     WGetEditInstance(), WStrHelpRoutine );
        } else {
            ftype = einfo->file_type;
        }
        ok = (ftype != WR_DONT_KNOW);
    }

    if( ok ) {
        if( ftype == WR_WIN_RC_STR ) {
            ok = WSaveObjectToRC( einfo, fname, false, false );
            if( ok ) {
                WGetInternalRESName( fname, resfile );
                if( einfo->info->is32bit ) {
                    rtype = WR_WINNTW_RES;
                } else {
                    rtype = WR_WIN16W_RES;
                }
            }
        } else {
            strcpy( resfile, fname );
            rtype = ftype;
        }
    }

    if( ok ) {
        if( einfo->info->symbol_file != NULL ) {
            idata2.next = idata;
            ok = WRSaveObjectAs( resfile, rtype, &idata2 );
        } else {
            ok = WRSaveObjectAs( resfile, rtype, idata );
        }
    }

    if( ok ) {
        if( got_name ) {
            if( einfo->file_name != NULL ) {
                WRMemFree( einfo->file_name );
            }
            einfo->file_name = fname;
            einfo->file_type = ftype;
            WSetEditTitle( einfo );
        }
    } else {
        if( fname != NULL && got_name ) {
            WRMemFree( fname );
        }
    }

    if( idata2.type != NULL ) {
        WRMemFree( idata2.type );
    }
    if( idata2.name != NULL) {
        WRMemFree( idata2.name );
    }

    return( ok );
}

bool WSaveObjectInto( WStringEditInfo *einfo, WRSaveIntoData *idata )
{
    char                *fname;
    WGetFileStruct      gf;
    bool                dup;
    WRFileType          ftype;
    bool                ok;

    fname = NULL;
    dup = false;

    ok = (einfo != NULL);

    if( ok ) {
        gf.file_name = NULL;
        gf.title = AllocRCString( W_SAVERESINTO );
        gf.filter = AllocRCString( W_SAVERESFILTER );
        WMassageFilter( gf.filter );
        fname = WGetOpenFileName( einfo->win, &gf );
        if( gf.title != NULL ) {
            FreeRCString( gf.title );
        }
        if( gf.filter != NULL ) {
            FreeRCString( gf.filter );
        }
        ok = (fname != NULL);
    }

    if( ok ) {
        ftype = WSelectFileType( einfo->win, fname, einfo->info->is32bit,
                                 true, WGetEditInstance(), WStrHelpRoutine );
        if( ftype == WR_WIN_RC_STR ) {
            ok = WSaveObjectToRC( einfo, fname, false, true );
        } else {
            ok = WRSaveObjectInto( fname, idata, &dup ) && !dup;
        }
    }

    if( dup ) {
        WDisplayErrorMsg( W_NORESOLVEDUPS );
    }

    if( fname != NULL ) {
        WRMemFree( fname );
    }

    return( ok );
}

WRSaveIntoData *WMakeSaveData( WStringTable *tbl )
{
    WRSaveIntoData      *node;
    WRSaveIntoData      *new;
    WStringBlock        *block;
    WResID              *tname;
    WResLangType        *lang;

    tname = WResIDFromNum( RESOURCE2INT( RT_STRING ) );
    if( tname == NULL ) {
        return( NULL );
    }

    lang = (WResLangType *)WRMemAlloc( sizeof( WResLangType ) );
    if( lang == NULL ) {
        WRMemFree( tname );
        return( NULL );
    }
    lang->lang = DEF_LANG;
    lang->sublang = DEF_SUBLANG;

    node = NULL;
    block = tbl->first_block;
    while( block != NULL ) {
        new = WInitSaveData( block, tname, lang );
        if( new == NULL ) {
            WRMemFree( tname );
            WFreeSaveIntoData( node );
            return( NULL );
        }
        if( node != NULL ) {
            new->next = node;
            node = new;
        } else {
            node = new;
        }
        block = block->next;
    }

    return( node );
}

WRSaveIntoData *WInitSaveData( WStringBlock *block, WResID *type, WResLangType *lang )
{
    size_t              size;
    WRSaveIntoData      *new;

    new = WAllocSaveIntoData();
    if( new == NULL ) {
        return( NULL );
    }

    new->name = WResIDFromNum( (block->blocknum >> 4) + 1 );
    if( new->name == NULL ) {
        WFreeSaveIntoData( new );
        return( NULL );
    }

    WRMakeDataFromStringBlock( &block->block, &new->data, &size, block->is32bit );
    if( new->data == NULL ) {
        WFreeSaveIntoData( new );
        return( NULL );
    }

    new->size = (uint_32)size;
    new->type = type;
    new->lang = *lang;
    new->MemFlags = block->MemFlags;

    return( new );
}

WRSaveIntoData *WAllocSaveIntoData( void )
{
    WRSaveIntoData *idata;

    idata = (WRSaveIntoData *)WRMemAlloc( sizeof( WRSaveIntoData ) );
    if( idata != NULL ) {
        memset( idata, 0, sizeof( WRSaveIntoData ) );
    }

    return( idata );
}

void WFreeSaveIntoData( WRSaveIntoData *idata )
{
    WRSaveIntoData *next;

    if( idata != NULL ) {
        if( idata->type != NULL ) {
            WRMemFree( idata->type );
        }
    }

    while( idata != NULL ) {
        next = idata->next;
        if( idata->name != NULL ) {
            WRMemFree( idata->name );
        }
        if( idata->data != NULL ) {
            WRMemFree( idata->data );
        }
        WRMemFree( idata );
        idata = next;
    }
}

bool WSaveSymbols( WStringEditInfo *einfo, WRHashTable *table, char **file_name,
                   bool prompt )
{
    char                *name;
    WGetFileStruct      gf;
    bool                ok;

    if( einfo == NULL || table == NULL || file_name == NULL ) {
        return( false );
    }

    if( WRIsDefaultHashTable( table ) ) {
        return( true );
    }

    ok = true;

    WSetWaitCursor( einfo->win, true );

    if( prompt || *file_name == NULL ) {
        gf.file_name = *file_name;
        gf.title = AllocRCString( W_SAVESYMTITLE );
        gf.filter = AllocRCString( W_SYMFILTER );
        WMassageFilter( gf.filter );
        name = WGetSaveFileName( einfo->win, &gf );
        if( gf.title != NULL ) {
            FreeRCString( gf.title );
        }
        if( gf.filter != NULL ) {
            FreeRCString( gf.filter );
        }
        ok = (name != NULL);
        if( ok ) {
            if( *file_name != NULL ) {
                WRMemFree( *file_name );
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

    WSetWaitCursor( einfo->win, false );

    return( ok );
}
