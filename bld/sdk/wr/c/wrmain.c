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


#include "wrglbl.h"
#include "wrmemi.h"
#include "wrimg.h"
#include "wrrdw16.h"
#include "wrrdwnt.h"
#include "wrrdres.h"
#include "wrsvres.h"
#include "wrsvexe.h"
#include "wridfile.h"
#include "wrtmpfil.h"
#include "wrstrdup.h"
//#include "wrfindti.h"
#include "wrmsg.h"
#include "wrclean.h"
#include "wrinfoi.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "jdlg.h"
#include "rcrtns.h"
#include "dllmain.h"
#include "wresdefn.h"
#include "reserr.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WREDoSaveObjectInto( WRInfo *, WRSaveIntoData *, bool * );
static bool WREDoSaveObjectAs( WRInfo *info, WRSaveIntoData *idata );
static bool WREDoSaveImageAs( WRInfo *info, WRSaveIntoData *idata, bool is_icon );
static bool WREDoSaveImageInto( WRInfo *info, WRSaveIntoData *idata, bool *dup, bool is_icon );
static bool WRTestReplace( WRInfo *, WRSaveIntoData * );
static bool WQueryMergeStrings( WResID * );
static bool WQueryReplaceObject( void );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE        WRInstance = NULL;
static int              ref_count = 0;


FILE *res_open( const char *file_name, wres_open_mode omode )
{
    FILE    *fp;

    switch( omode ) {
    default:
    case WRES_OPEN_RO:
        fp = fopen( file_name, "rb" );
        break;
    case WRES_OPEN_RW:
    case WRES_OPEN_NEW:
        fp = fopen( file_name, "wb" );
        break;
    case WRES_OPEN_TMP:
        fp = tmpfile();
        break;
    }
    if( fp == NULL )
        WRES_ERROR( WRS_OPEN_FAILED );
    return( fp );
}

bool res_close( FILE *fp )
{
    return( fclose( fp ) != 0 );
}

size_t res_read( FILE *fp, void *buf, size_t size )
{
    return( fread( buf, 1, size, fp ) );
}

size_t res_write( FILE *fp, const void *buf, size_t size )
{
    return( fwrite( buf, 1, size, fp ) );
}

bool res_seek( FILE *fp, long pos, int where )
{
    if( where == SEEK_SET ) {
        /* fool the wres library into thinking that the resource information starts at offset 0 */
        return( fseek( fp, pos + WResFileShift, where ) != 0 );
    }
    return( fseek( fp, pos, where ) != 0 );
}

long res_tell( FILE *fp )
{
    return( ftell( fp ) );
}

bool res_ioerr( FILE *fp, size_t rc )
/*****************************************/
{
    /* unused parameters */ (void)rc;

    return( ferror( fp ) != 0 );
}

/* set the WRES library to use compatible functions */
WResSetRtns(res_open,res_close,res_read,res_write,res_seek,res_tell,res_ioerr,RESALLOC,RESFREE);

#ifdef __NT__

BOOL WINAPI DllMain( HINSTANCE inst, DWORD dwReason, LPVOID lpReserved )
{
    /* unused parameters */ (void)lpReserved;

    switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
        WRInstance = inst;
        WRInitRcGlobal( inst );
        ref_count = 0;
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        /* do nothing here */
        break;
    }

    return( TRUE );
}

#else

int WINAPI LibMain( HINSTANCE inst, WORD dataseg, WORD heapsize, LPSTR cmdline )
{
    /* unused parameters */ (void)dataseg; (void)heapsize; (void)cmdline;

    WRInstance = inst;
    WRInitRcGlobal( inst );

    ref_count = 0;

    return( TRUE );
}

int WINAPI WEP( int parm )
{
    /* unused parameters */ (void)parm;

    return( TRUE );
}

#endif

HINSTANCE WRGetInstance( void )
{
    return( WRInstance );
}

void WRAPI WRInit( void )
{
    if( ref_count == 0 ) {
        WRDialogMsgInit();
        WRCtl3DInit( WRInstance );
        WRMemOpen();
        WRInitDisplayError( WRInstance );
        JDialogInit();
    }
    ref_count++;
}

void WRAPI WRFini( void )
{
    ref_count--;
    if( ref_count == 0 ) {
        WRDialogMsgFini();
        WRCtl3DFini( WRInstance );
        JDialogFini();
        WRMemClose();
    }
}

WRInfo * WRAPI WRLoadResource( const char *name, WRFileType type )
{
    WRInfo  *info;
    bool    ok;

    info = WRAllocWRInfo();

    if( info == NULL ) {
        return( NULL );
    }

    if( type == WR_DONT_KNOW ) {
        type = WRIdentifyFile( name );
    }

#ifndef __NT__
    if( WRIs32Bit( type ) ) {
        WRDisplayErrorMsg( WR_NOLOAD32IN16 );
        WRFreeWRInfo( info );
        return( NULL );
    }
#endif

    info->file_type = type;
    if( (info->file_name = WRStrDup( name )) == NULL ) {
        WRFreeWRInfo( info );
        return( NULL );
    }

    switch( type ) {
    case WR_WIN_BITMAP:
        ok = WRLoadBitmapFile( info );
        break;

    case WR_WIN_ICON:
        ok = WRLoadIconFile( info );
        break;

    case WR_WIN_CURSOR:
        ok = WRLoadCursorFile( info );
        break;

    case WR_WINNTM_RES:
    case WR_WIN16M_RES:
    case WR_WINNTW_RES:
    case WR_WIN16W_RES:
        ok = WRLoadResourceFromRES( info );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ok = WRLoadResourceFromWin16EXE( info );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ok = WRLoadResourceFromWinNTEXE( info );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ok = WRLoadResourceFrom_RC( info );
        break;

    case WR_INVALID_FILE:
        ok = false;
        break;

    case WR_DONT_KNOW:
    default:
        ok = false;
        WRDisplayErrorMsg( WR_BADFILETYPE );
        break;
    }

    if( ok ) {
        if( info->internal_filename != NULL ) {
            ok = WRCopyFileToTemp( info, info->internal_filename );
        } else {
            ok = WRCopyFileToTemp( info, info->file_name );
        }
    }

    if( !ok ) {
        WRFreeWRInfo( info );
        return( NULL );
    }

    return( info );
}

bool WRAPI WRSaveResource( WRInfo *info, bool backup )
{
    bool        ok;
    char        *tmp;
    char        *name;
    pgroup2     pg;

    if( info->save_name == NULL ) {
        return( false );
    }

#ifndef __NT__
    if( WRIs32Bit( info->save_type ) ) {
        WRDisplayErrorMsg( WR_NOSAVE32IN16 );
        return( false );
    }
#endif

    if( info->internal_filename != NULL ) {
        name = info->internal_filename;
    } else {
        name = info->file_name;
    }

    /* if the save and file names are the same then use a tmp file */
    if( name != NULL && stricmp( name, info->save_name ) == 0 ) {
        tmp = info->save_name;
        _splitpath2( info->save_name, pg.buffer, NULL, NULL, NULL, &pg.ext );
        info->save_name = WRGetTempFileName( pg.ext );
        if( info->save_name == NULL ) {
            info->save_name = tmp;
            return( false );
        }
    } else {
        tmp = NULL;
    }

    switch( info->save_type ) {
    case WR_WIN_BITMAP:
        ok = WRSaveBitmapResource( info, backup );
        break;

    case WR_WIN_ICON:
        ok = WRSaveIconResource( info, backup );
        break;

    case WR_WIN_CURSOR:
        ok = WRSaveCursorResource( info, backup );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ok = WRSaveResourceTo_RC( info, backup );
        break;

    case WR_WIN16M_RES:
    case WR_WIN16W_RES:
    case WR_WINNTM_RES:
    case WR_WINNTW_RES:
        ok = WRSaveResourceToRES( info, backup );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ok = WRSaveResourceToWin16EXE( info, backup );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ok = WRSaveResourceToWinNTEXE( info, backup );
        break;

    case WR_DONT_KNOW:
    case WR_INVALID_FILE:
    default:
        WRDisplayErrorMsg( WR_BADSAVETYPE );
        ok = false;
        break;
    }

    if( ok && info->dir != NULL ) {
        ok = WRRelinkInfo( info );
    }

    if( tmp != NULL ) {
        ok = ( ok && WRRenameFile( tmp, info->save_name ) );
        MemFree( info->save_name );
        info->save_name = tmp;
    }

    return( ok );
}

bool WRAPI WRUpdateTmp( WRInfo *info )
{
    bool        ok;
    char        *tsave;
    WRFileType  ttype;
    pgroup2     pg;

    if( info == NULL || info->file_name == NULL ) {
        return( false );
    }

    if( info->internal_filename != NULL ) {
        _splitpath2( info->internal_filename, pg.buffer, NULL, NULL, NULL, &pg.ext );
    } else {
        _splitpath2( info->file_name, pg.buffer, NULL, NULL, NULL, &pg.ext );
    }

    if( info->tmp_file == NULL ) {
        info->tmp_file = WRGetTempFileName( pg.ext );
        if( info->tmp_file == NULL ) {
            return( false );
        }
    }

    tsave = info->save_name;
    ttype = info->save_type;
    if( info->internal_type != WR_DONT_KNOW ) {
        info->save_type = info->internal_type;
    } else {
        info->save_type = info->file_type;
    }
    info->save_name = WRGetTempFileName( pg.ext );
    if( info->save_name == NULL ) {
        info->save_name = tsave;
        info->save_type = ttype;
        return( false );
    }

    switch( info->save_type ) {
    case WR_WIN_BITMAP:
        ok = WRSaveBitmapResource( info, false );
        break;

    case WR_WIN_ICON:
        ok = WRSaveIconResource( info, false );
        break;

    case WR_WIN_CURSOR:
        ok = WRSaveCursorResource( info, false );
        break;

    case WR_WIN16M_RES:
    case WR_WIN16W_RES:
    case WR_WINNTM_RES:
    case WR_WINNTW_RES:
        ok = WRSaveResourceToRES( info, false );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ok = WRSaveResourceToWin16EXE( info, false );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ok = WRSaveResourceToWinNTEXE( info, false );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ok = false;
        break;

    case WR_DONT_KNOW:
    case WR_INVALID_FILE:
    default:
        WRDisplayErrorMsg( WR_BADSAVETYPE );
        ok = false;
        break;
    }

    if( ok ) {
        ok = WRRelinkInfo( info );
    }

    if( ok ) {
        ok = WRRenameFile( info->tmp_file, info->save_name );
        if( !ok ) {
            ok = WRDeleteFile( info->save_name );
        }
    }

    MemFree( info->save_name );
    info->save_name = tsave;
    info->save_type = ttype;

    return( ok );
}

bool WRAPI WRSaveObjectAs( const char *file, WRFileType file_type, WRSaveIntoData *idata )
{
    WRInfo      *info;
    long        type;
    bool        ok;

    info = NULL;

    ok = (file != NULL && idata->type != NULL && idata->name != NULL &&
          idata->data != NULL &&
          (file_type == WR_WIN_BITMAP || file_type == WR_WIN_CURSOR ||
           file_type == WR_WIN_ICON || file_type == WR_WIN_RC ||
           file_type == WR_WIN_RC_STR || file_type == WR_WIN_RC_MENU ||
           file_type == WR_WIN_RC_ACCEL || file_type == WR_WIN_RC_DLG ||
           file_type == WR_WIN16M_RES || file_type == WR_WIN16W_RES ||
           file_type == WR_WINNTM_RES || file_type == WR_WINNTW_RES));

    if( ok ) {
        info = WRAllocWRInfo();
        ok = (info != NULL);
    }

    if( ok ) {
        ok = ((info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        info->save_type = file_type;
        info->save_name = (char *)file;
    }

    while( ok && idata != NULL ) {
        type = WResIDToNum( idata->type );
        if( type == RESOURCE2INT( RT_GROUP_ICON ) ) {
            ok = WREDoSaveImageAs( info, idata, true );
        } else if( type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
            ok = WREDoSaveImageAs( info, idata, false );
        } else {
            ok = WREDoSaveObjectAs( info, idata );
        }
        idata = idata->next;
    }

    if( ok ) {
        ok = WRSaveResource( info, true );
    }

    if( info != NULL ) {
        info->save_name = NULL;
        WRCleanDir( info->dir );
        WRFreeWRInfo( info );
    }

    return( ok );
}

bool WRAPI WRSaveObjectInto( const char *file, WRSaveIntoData *idata, bool *dup )
{
    WRInfo      *info;
    char        *tmp_file;
    pgroup2     pg;
    long        type;
    bool        ok;

    info = NULL;
    tmp_file = NULL;

    ok = (file != NULL && idata != NULL && dup != NULL);

    if( ok ) {
        info = WRLoadResource( file, WR_DONT_KNOW );
        ok = (info != NULL);
    }

    if( ok ) {
        _splitpath2( info->file_name, pg.buffer, NULL, NULL, NULL, &pg.ext );
        ok = ((tmp_file = WRGetTempFileName( pg.ext )) != NULL);
    }

    if( ok ) {
        info->save_type = info->file_type;
        info->save_name = tmp_file;
    }

    // loop thru all of the data
    while( ok && idata != NULL ) {
        type = WResIDToNum( idata->type );
        if( type == RESOURCE2INT( RT_GROUP_ICON ) ) {
            ok = WREDoSaveImageInto( info, idata, dup, true );
        } else if( type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
            ok = WREDoSaveImageInto( info, idata, dup, false );
        } else {
            ok = WREDoSaveObjectInto( info, idata, dup );
        }
        idata = idata->next;
    }

    if( ok ) {
        ok = WRSaveResource( info, true );
    }

    if( ok ) {
        ok = WRRenameFile( info->file_name, info->save_name );
    }

    if( info != NULL ) {
        info->save_name = NULL;
        WRCleanDir( info->dir );
        WRFreeWRInfo( info );
    }

    if( tmp_file != NULL ) {
        MemFree( tmp_file );
    }

    return( ok );
}

bool WRAPI WRFindAndSetData( WResDir dir, WResID *type, WResID *name,
                                WResLangType *lang, void *data )
{
    WResLangNode    *lnode;
    bool            ok;

    ok = (dir != NULL && type != NULL && name != NULL && lang != NULL && data != NULL);

    lnode = NULL;
    if( ok ) {
        lnode = WRFindLangNode( dir, type, name, lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        lnode->data = data;
    }

    return( ok );
}

WResLangNode *WRAPI WRFindLangNode( WResDir dir, WResID *type,
                                         WResID *name, WResLangType *lang )
{
    WResTypeNode    *tnode;
    WResResNode     *rnode;
    WResLangNode    *lnode;
    bool            ok;

    ok = (dir != NULL && type != NULL && name != NULL && lang != NULL);

    tnode = NULL;
    if( ok ) {
        tnode = WRFindTypeNodeFromWResID( dir, type );
        ok = (tnode != NULL);
    }

    rnode = NULL;
    if( ok ) {
        rnode = WRFindResNodeFromWResID( tnode, name );
        ok = (rnode != NULL);
    }

    lnode = NULL;
    if( ok ) {
        lnode = WRFindLangNodeFromLangType( rnode, lang );
        ok = (lnode != NULL);
    }

    return( lnode );
}

bool WREDoSaveObjectAs( WRInfo *info, WRSaveIntoData *idata )
{
    bool          ok;

    ok = (info != NULL && info->dir != NULL && idata != NULL && idata->type != NULL &&
          idata->name != NULL && idata->data != NULL);

    if( ok ) {
        ok = !WResAddResource( idata->type, idata->name, idata->MemFlags, 0,
                               idata->size, info->dir, &idata->lang, NULL );
    }

    if( ok ) {
        ok = WRFindAndSetData( info->dir, idata->type, idata->name,
                               &idata->lang, idata->data );
    }

    return( ok );
}

bool WREDoSaveImageAs( WRInfo *info, WRSaveIntoData *idata, bool is_icon )
{
    bool                ok;
    BYTE                *data;
    size_t              size;
    WResLangNode        *lnode;

    data = NULL;
    ok = (info != NULL && info->dir != NULL && idata != NULL && idata->type != NULL &&
          idata->name != NULL && idata->data != NULL && idata->info != NULL);

    lnode = NULL;
    if( ok ) {
        lnode = WRFindLangNode( idata->info->dir, idata->type, idata->name, &idata->lang );
        ok = (lnode != NULL);
    }

    size = 0;
    if( ok ) {
        if( is_icon ) {
            ok = WRCreateIconData( idata->info, lnode, &data, &size );
        } else {
            ok = WRCreateCursorData( idata->info, lnode, &data, &size );
        }
    }

    if( ok ) {
        ok = !WResAddResource( idata->type, idata->name, idata->MemFlags, 0,
                               idata->size, info->dir, &idata->lang, NULL );
    }

    if( ok ) {
        lnode = WRFindLangNode( info->dir, idata->type, idata->name, &idata->lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        if( is_icon ) {
            ok = WRCreateIconEntries( info, lnode, data, size );
        } else {
            ok = WRCreateCursorEntries( info, lnode, data, size );
        }
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );
}

bool WREDoSaveObjectInto( WRInfo *info, WRSaveIntoData *idata, bool *dup )
{
    bool ok;
    bool replace_nixed;

    replace_nixed = false;

    ok = (info != NULL && idata != NULL && idata->type != NULL && idata->name != NULL &&
          idata->data != NULL && dup != NULL);

    if( ok ) {
        ok = WRTestReplace( info, idata );
        replace_nixed = !ok;
    }

    if( ok ) {
        ok = !WResAddResource( idata->type, idata->name, idata->MemFlags, 0,
                               idata->size, info->dir, &idata->lang, dup );
        ok = ok && !*dup;
    }

    if( ok ) {
        ok = WRFindAndSetData( info->dir, idata->type, idata->name,
                               &idata->lang, idata->data );
    }

    if( replace_nixed ) {
        return( true );
    }

    return( ok );
}

bool WREDoSaveImageInto( WRInfo *info, WRSaveIntoData *idata, bool *dup, bool is_icon )
{
    BYTE                *data;
    size_t              size;
    WResLangNode        *lnode;
    bool                replace_nixed;
    bool                ok;

    replace_nixed = false;
    data = NULL;
    ok = (info != NULL && info->dir != NULL && idata != NULL && idata->type != NULL &&
          idata->name != NULL && idata->data != NULL && idata->info != NULL && dup != NULL);

    if( ok ) {
        ok = WRTestReplace( info, idata );
        replace_nixed = !ok;
    }

    lnode = NULL;
    if( ok ) {
        lnode = WRFindLangNode( idata->info->dir, idata->type, idata->name, &idata->lang );
        ok = (lnode != NULL);
    }

    size = 0;
    if( ok ) {
        if( is_icon ) {
            ok = WRCreateIconData( idata->info, lnode, &data, &size );
        } else {
            ok = WRCreateCursorData( idata->info, lnode, &data, &size );
        }
    }

    if( ok ) {
        ok = !WResAddResource( idata->type, idata->name, idata->MemFlags, 0,
                               idata->size, info->dir, &idata->lang, dup );
        ok = ok && !*dup;
    }

    if( ok ) {
        lnode = WRFindLangNode( info->dir, idata->type, idata->name, &idata->lang );
        ok = (lnode != NULL);
    }

    if( ok ) {
        if( is_icon ) {
            ok = WRCreateIconEntries( info, lnode, data, size );
        } else {
            ok = WRCreateCursorEntries( info, lnode, data, size );
        }
    }

    if( data != NULL ) {
        MemFree( data );
    }

    if( replace_nixed ) {
        return( true );
    }

    return( ok );
}

bool WQueryReplaceObject( void )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    title = WRAllocRCString( WR_COPYRESINTO );
    text = WRAllocRCString( WR_REPLACERES );

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;

    ret = MessageBox( HWND_DESKTOP, text, title, style );

    if( title != NULL ) {
        WRFreeRCString( title );
    }

    if( text != NULL ) {
        WRFreeRCString( text );
    }

    if( ret == IDNO ) {
        return( false );
    }

    return( true );
}

bool WQueryMergeStrings( WResID *rname )
{
    int         ret;
    UINT        style;
    char        *text;
    char        *title;
    char        *str;

    ret = IDNO;
    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;

    title = WRAllocRCString( WR_COPYRESINTO );
    text = WRAllocRCString( WR_REPLACESTR );

    if( text != NULL ) {
        str = (char *)MemAlloc( strlen( text ) + 1 + 10 ); // space for 10 digits
        if( str != NULL ) {
            sprintf( str, text, rname->ID.Num );
            ret = MessageBox( HWND_DESKTOP, str, title, style );
            MemFree( str );
        }
        WRFreeRCString( text );
    }

    if( title != NULL ) {
        WRFreeRCString( title );
    }

    if( ret == IDNO ) {
        return( false );
    }

    return( true );
}

// this function returns true if the save into may continue
bool WRTestReplace( WRInfo *info, WRSaveIntoData *idata )
{
    WResTypeNode    *tnode;
    WResResNode     *rnode;
    WResLangNode    *lnode;
    long            type;
    void            *data;
    uint_32         size;
    bool            strings;

    if( info == NULL || info->dir == NULL || idata == NULL || idata->type == NULL ||
        idata->name == NULL ) {
        return( false );
    }

    type = WResIDToNum( idata->type );

    strings = ( type == RESOURCE2INT( RT_STRING ) );

    tnode = WRFindTypeNodeFromWResID( info->dir, idata->type );
    if( tnode == NULL ) {
        return( true );
    }

    rnode = WRFindResNodeFromWResID( tnode, idata->name );
    if( rnode == NULL ) {
        return( true );
    }

    lnode = WRFindLangNodeFromLangType( rnode, &idata->lang );
    if( lnode == NULL ) {
        return( true );
    }

    if( strings ) {
        if( !WQueryMergeStrings( idata->name ) ) {
            return( false );
        }
        data = WRLoadResData( info->file_name, lnode->Info.Offset, lnode->Info.Length );
        size = lnode->Info.Length;
        if( !WRMergeStringData( &data, &size, idata->data, idata->size,
                                WRIs32Bit( info->save_type ), true ) ) {
            if( data != NULL ) {
                MemFree( data );
            }
            return( false );
        }
        if( idata->data != NULL ) {
            MemFree( idata->data );
        }
        idata->data = data;
        idata->size = size;
    } else {
        if( !WQueryReplaceObject() ) {
            return( false );
        }
    }

    if( type == RESOURCE2INT( RT_GROUP_ICON ) || type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
        if( !WRDeleteGroupImages( info, lnode, type ) ) {
            return( false );
        }
    }

    if( !WRRemoveLangNodeFromDir( info->dir, &tnode, &rnode, &lnode ) ) {
        return( false );
    }

    return( true );
}
