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


#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include "wglbl.h"
#include "wmem.h"
#include "wwait.h"
#include "wrdll.h"
#include "wselft.h"
#include "wgetfn.h"
#include "winst.h"
#include "wmsg.h"
#include "wmain.h"
#include "weditsym.h"
#include "wsvobj.h"
#include "wmsgfile.h"
#include "wstr2rc.h"

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
static Bool             WSaveObjectAs     ( Bool, WStringEditInfo *,
                                            WRSaveIntoData * );
static Bool             WSaveObjectInto   ( WStringEditInfo *,
                                            WRSaveIntoData * );
static WRSaveIntoData   *WMakeSaveData    ( WStringTable *tbl );
static void             WFreeSaveIntoData ( WRSaveIntoData *idata );
static WRSaveIntoData   *WAllocSaveIntoData( void );
static WRSaveIntoData   *WInitSaveData    ( WStringBlock *, WResID *,
                                            WResLangType * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static Bool WSaveObjectToRC( WStringEditInfo *einfo, char *filename,
                             Bool shadow, Bool append )
{
    char        fn_path[ _MAX_PATH ];
    char        fn_drive[_MAX_DRIVE];
    char        fn_dir[_MAX_DIR];
    char        fn_name[_MAX_FNAME];
    char        fn_ext[_MAX_EXT+1];

    if( !einfo || !filename ) {
        return( FALSE );
    }

    if( shadow ) {
        _splitpath( filename, fn_drive, fn_dir, fn_name, fn_ext );
        strcpy( fn_ext, ".str" );
        _makepath( fn_path, fn_drive, fn_dir, fn_name, fn_ext );
    } else {
        strcpy( fn_path, filename );
    }

    if( WRFileExists( fn_path ) ) {
        WRBackupFile( fn_path, TRUE );
    }

    if( !WWriteStringToRC( einfo, fn_path, append ) ) {
        return( FALSE );
    }

    return( TRUE );
}

Bool WSaveObject( WStringEditInfo *einfo, Bool get_name, Bool save_into )
{
    WRSaveIntoData      *idata;
    Bool                ok;

    idata = NULL;

    WSetWaitCursor( einfo->win, TRUE );

    ok = ( einfo && einfo->tbl );

    if( ok ) {
        if( !WRIsDefaultHashTable( einfo->info->symbol_table ) &&
            ( get_name || WRIsHashTableDirty( einfo->info->symbol_table ) ) ) {
            if( einfo->info->symbol_file == NULL ) {
                char    *fname;
                if( !einfo->file_name ) {
                    fname = einfo->info->file_name;
                } else {
                    fname = einfo->file_name;
                }
                einfo->info->symbol_file = WCreateSymName( fname );
            }
            ok = WSaveSymbols( einfo, einfo->info->symbol_table,
                               &einfo->info->symbol_file, get_name );
        }
    }

    if( ok ) {
        idata = WMakeSaveData( einfo->tbl );
        ok = ( idata != NULL );
    }

    if( ok ) {
        if( save_into ) {
            ok = WSaveObjectInto( einfo, idata );
        } else {
            ok = WSaveObjectAs( get_name, einfo, idata );
        }
    }

    if( ok && einfo->info->stand_alone ) {
        einfo->info->modified = FALSE;
    }

    if( idata ) {
        WFreeSaveIntoData( idata );
    }

    WSetWaitCursor( einfo->win, FALSE );

    return( ok );
}

Bool WSaveObjectAs( Bool get_name, WStringEditInfo *einfo,
                    WRSaveIntoData *idata )
{
    char                resfile[ _MAX_PATH ];
    char                *fname;
    WRFileType          ftype;
    WRFileType          rtype;
    WGetFileStruct      gf;
    WRSaveIntoData      idata2;
    WResLangType        lang;
    Bool                got_name;
    Bool                ok;

    lang.lang    = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    fname    = NULL;
    got_name = FALSE;

    ok = ( einfo != NULL );

    if( ok ) {
        memset( &idata2, 0, sizeof( idata2 ) );
        if( einfo->info->symbol_file ) {
            idata2.next = NULL;
            idata2.type = WResIDFromNum( (long) RT_RCDATA );
            idata2.name = WResIDFromStr( "DLGINCLUDE" );
            idata2.data = einfo->info->symbol_file;
            idata2.lang = lang;
            idata2.size = strlen( einfo->info->symbol_file ) + 1;
            idata2.MemFlags = MEMFLAG_DISCARDABLE;
        }
    }

    if ( ok ) {
        if ( !einfo->file_name || get_name ) {
            gf.file_name = NULL;
            gf.title     = WAllocRCString( W_SAVERESAS );
            gf.filter    = WAllocRCString( W_SAVERESFILTER );
            WMassageFilter( gf.filter );
            fname        = WGetSaveFileName( einfo->win, &gf );
            if( gf.title ) {
                WFreeRCString( gf.title );
            }
            if( gf.filter ) {
                WFreeRCString( gf.filter );
            }
            if( fname ) {
                got_name = TRUE;
            }
        } else {
            fname = einfo->file_name;
        }
        ok = ( fname != NULL );
    }

    if( ok ) {
        if( got_name ) {
            ftype = WSelectFileType( einfo->win, fname,
                                     einfo->info->is32bit, TRUE,
                                     WGetEditInstance(), WStrHelpRoutine );
        } else {
            ftype = einfo->file_type;
        }
        ok = ( ftype != WR_DONT_KNOW );
    }

    if( ok ) {
        if( ftype == WR_WIN_RC_STR ) {
            ok = WSaveObjectToRC( einfo, fname, FALSE, FALSE );
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
        if( einfo->info->symbol_file ) {
            idata2.next = idata;
            ok = WRSaveObjectAs( resfile, rtype, &idata2 );
        } else {
            ok = WRSaveObjectAs( resfile, rtype, idata );
        }
    }

    if ( ok ) {
        if ( got_name ) {
            if ( einfo->file_name ) {
                WMemFree ( einfo->file_name );
            }
            einfo->file_name = fname;
            einfo->file_type = ftype;
            WSetEditTitle ( einfo );
        }
    } else {
        if ( fname && got_name ) {
            WMemFree ( fname );
        }
    }

    if( idata2.type ) {
        WMemFree( idata2.type );
    }
    if( idata2.name ) {
        WMemFree( idata2.name );
    }

    return ( ok );
}

Bool WSaveObjectInto( WStringEditInfo *einfo, WRSaveIntoData *idata )
{
    char                *fname;
    WGetFileStruct      gf;
    int                 dup;
    WRFileType          ftype;
    Bool                ok;

    fname = NULL;
    dup   = FALSE;

    ok = ( einfo != NULL );

    if( ok ) {
        gf.file_name = NULL;
        gf.title     = WAllocRCString( W_SAVERESINTO );
        gf.filter    = WAllocRCString( W_SAVERESFILTER );
        WMassageFilter( gf.filter );
        fname        = WGetOpenFileName ( einfo->win, &gf );
        if( gf.title ) {
            WFreeRCString( gf.title );
        }
        if( gf.filter ) {
            WFreeRCString( gf.filter );
        }
        ok = ( fname != NULL );
    }

    if( ok ) {
        ftype = WSelectFileType( einfo->win, fname, einfo->info->is32bit,
                                 TRUE, WGetEditInstance(), WStrHelpRoutine );
        if( ftype == WR_WIN_RC_STR ) {
            ok = WSaveObjectToRC( einfo, fname, FALSE, TRUE );
        } else {
            ok = WRSaveObjectInto( fname, idata, &dup ) && !dup;
        }
    }

    if( dup ) {
        WDisplayErrorMsg( W_NORESOLVEDUPS );
    }

    if( fname ) {
        WMemFree( fname );
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

    tname = WResIDFromNum( (long) RT_STRING );
    if( tname == NULL ) {
        return( NULL );
    }

    lang = (WResLangType *) WMemAlloc( sizeof(WResLangType) );
    if( lang == NULL ) {
        WMemFree( tname );
        return( NULL );
    }
    lang->lang    = DEF_LANG;
    lang->sublang = DEF_SUBLANG;

    node = NULL;
    block  = tbl->first_block;
    while( block ) {
        new = WInitSaveData( block, tname, lang );
        if( new == NULL ) {
            WMemFree( tname );
            WFreeSaveIntoData( node );
            return( NULL );
        }
        if( node ) {
            new->next = node;
            node = new;
        } else {
            node = new;
        }
        block = block->next;
    }

    return( node );
}

WRSaveIntoData *WInitSaveData( WStringBlock *block, WResID *type,
                               WResLangType *lang )
{
    int                 size;
    WRSaveIntoData      *new;

    new = WAllocSaveIntoData();
    if( new == NULL ) {
        return( NULL );
    }

    new->name = WResIDFromNum( ( block->blocknum >> 4 ) + 1 );
    if( new->name == NULL ) {
        WFreeSaveIntoData( new );
        return( NULL );
    }

    WRMakeDataFromStringBlock( &block->block, &new->data, &size, block->is32bit );
    if( new->data == NULL ) {
        WFreeSaveIntoData( new );
        return( NULL );
    }

    new->size = size;
    new->type = type;
    new->lang = *lang;
    new->MemFlags = block->MemFlags;

    return( new );
}

WRSaveIntoData *WAllocSaveIntoData( void )
{
    WRSaveIntoData *idata;

    idata = (WRSaveIntoData *) WMemAlloc( sizeof(WRSaveIntoData) );
    if( idata ) {
        memset( idata, 0, sizeof(WRSaveIntoData) );
    }

    return( idata );
}

void WFreeSaveIntoData( WRSaveIntoData *idata )
{
    WRSaveIntoData *next;

    if( idata ) {
        if( idata->type ) {
            WMemFree( idata->type );
        }
    }

    while( idata ) {
        next = idata->next;
        if( idata->name ) {
            WMemFree( idata->name );
        }
        if( idata->data ) {
            WMemFree( idata->data );
        }
        WMemFree( idata );
        idata = next;
    }
}

Bool WSaveSymbols( WStringEditInfo *einfo, WRHashTable *table, char **file_name,
                   Bool prompt )
{
    char                *name;
    WGetFileStruct      gf;
    Bool                ok;

    if( !einfo || !table || !file_name ) {
        return( FALSE );
    }

    if( WRIsDefaultHashTable( table ) ) {
        return( TRUE );
    }

    ok = TRUE;

    WSetWaitCursor( einfo->win, TRUE );

    if( prompt || !*file_name ) {
        gf.file_name = *file_name;
        gf.title     = WAllocRCString( W_SAVESYMTITLE );
        gf.filter    = WAllocRCString( W_SYMFILTER );
        WMassageFilter( gf.filter );
        name = WGetSaveFileName( einfo->win, &gf );
        if( gf.title ) {
            WFreeRCString( gf.title );
        }
        if( gf.filter ) {
            WFreeRCString( gf.filter );
        }
        ok = ( name != NULL );
        if( ok ) {
            if( *file_name != NULL ) {
                WMemFree( *file_name );
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

    WSetWaitCursor( einfo->win, FALSE );

    return( ok );
}

