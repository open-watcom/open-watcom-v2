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


#include <wwindows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "watcom.h"
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
#include "wrfindti.h"
#include "wrmsg.h"
#include "wrclean.h"
#include "wrinfoi.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "jdlg.h"
#include "rcrtns.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static int  WREDoSaveObjectInto( WRInfo *, WRSaveIntoData *, int * );
static int  WREDoSaveObjectAs( WRInfo *info, WRSaveIntoData *idata );
static int  WREDoSaveImageAs( WRInfo *info, WRSaveIntoData *idata, int is_icon );
static int  WREDoSaveImageInto( WRInfo *info, WRSaveIntoData *idata, int *dup, int is_icon );
static int  WRTestReplace( WRInfo *, WRSaveIntoData * );
static int  WQueryMergeStrings( WResID * );
static int  WQueryReplaceObject( void );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE        WRInstance = NULL;
static int              ref_count = 0;


/* set the WRES library to use compatible functions */
WResSetRtns(RCOPEN,RCCLOSE,RCREAD,RCWRITE,RCSEEK,RCTELL,RCALLOC,RCFREE);

#ifdef __NT__

BOOL WINAPI DllMain( HINSTANCE inst, DWORD dwReason, LPVOID lpReserved )
{
    _wtouch( lpReserved );

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
    _wtouch( dataseg );
    _wtouch( heapsize );
    _wtouch( cmdline );

    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;

    WRInstance = inst;
    WRInitRcGlobal( inst );

    ref_count = 0;

    return( TRUE );
}

int WINAPI WEP( int parm )
{
    _wtouch( parm );

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
        WRMemPrtList();
        WRMemClose();
    }
}

WRInfo * WRAPI WRLoadResource( const char *name, WRFileType type )
{
    WRInfo  *info;
    int     ret;

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
        ret = WRLoadBitmapFile( info );
        break;

    case WR_WIN_ICON:
        ret = WRLoadIconFile( info );
        break;

    case WR_WIN_CURSOR:
        ret = WRLoadCursorFile( info );
        break;

    case WR_WINNTM_RES:
    case WR_WIN16M_RES:
    case WR_WINNTW_RES:
    case WR_WIN16W_RES:
        ret = WRLoadResourceFromRES( info );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ret = WRLoadResourceFromWin16EXE( info );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ret = WRLoadResourceFromWinNTEXE( info );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ret = WRLoadResourceFrom_RC( info );
        break;

    case WR_INVALID_FILE:
        ret = FALSE;
        break;

    case WR_DONT_KNOW:
    default:
        ret = FALSE;
        WRDisplayErrorMsg( WR_BADFILETYPE );
        break;
    }

    if( ret ) {
        if( info->internal_filename != NULL ) {
            ret = WRCopyFileToTemp( info, info->internal_filename );
        } else {
            ret = WRCopyFileToTemp( info, info->file_name );
        }
    }

    if( !ret ) {
        WRFreeWRInfo( info );
        return( NULL );
    }

    return( info );
}

int WRAPI WRSaveResource( WRInfo *info, int backup )
{
    int     ret;
    char    *tmp;
    char    *name;
    char    ext[_MAX_EXT];

    if( info->save_name == NULL ) {
        return( FALSE );
    }

#ifndef __NT__
    if( WRIs32Bit( info->save_type ) ) {
        WRDisplayErrorMsg( WR_NOSAVE32IN16 );
        return( FALSE );
    }
#endif

    if( info->internal_filename != NULL ) {
        name = info->internal_filename;
    } else {
        name = info->file_name;
    }

    /* if the save and file names are the same then use a tmp file */
    if( name != NULL && !stricmp( name, info->save_name ) ) {
        tmp = info->save_name;
        _splitpath( info->save_name, NULL, NULL, NULL, ext );
        info->save_name = WRGetTempFileName( ext );
        if( info->save_name == NULL ) {
            info->save_name = tmp;
            return( FALSE );
        }
    } else {
        tmp = NULL;
    }

    switch( info->save_type ) {
    case WR_WIN_BITMAP:
        ret = WRSaveBitmapResource( info, backup );
        break;

    case WR_WIN_ICON:
        ret = WRSaveIconResource( info, backup );
        break;

    case WR_WIN_CURSOR:
        ret = WRSaveCursorResource( info, backup );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ret = WRSaveResourceTo_RC( info, backup );
        break;

    case WR_WIN16M_RES:
    case WR_WIN16W_RES:
    case WR_WINNTM_RES:
    case WR_WINNTW_RES:
        ret = WRSaveResourceToRES( info, backup );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ret = WRSaveResourceToWin16EXE( info, backup );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ret = WRSaveResourceToWinNTEXE( info, backup );
        break;

    case WR_DONT_KNOW:
    case WR_INVALID_FILE:
    default:
        WRDisplayErrorMsg( WR_BADSAVETYPE );
        ret = FALSE;
        break;
    }

    if( ret && info->dir != NULL ) {
        ret = WRRelinkInfo( info );
    }

    if( tmp != NULL ) {
        ret = ( ret && WRRenameFile( tmp, info->save_name ) );
        MemFree( info->save_name );
        info->save_name = tmp;
    }

    return( ret );
}

int WRAPI WRUpdateTmp( WRInfo *info )
{
    int         ret;
    char        *tsave;
    WRFileType  ttype;
    char        ext[_MAX_EXT];

    if( info == NULL || info->file_name == NULL ) {
        return( FALSE );
    }

    if( info->internal_filename != NULL ) {
        _splitpath( info->internal_filename, NULL, NULL, NULL, ext );
    } else {
        _splitpath( info->file_name, NULL, NULL, NULL, ext );
    }

    if( info->tmp_file == NULL ) {
        info->tmp_file = WRGetTempFileName( ext );
        if( info->tmp_file == NULL ) {
            return( FALSE );
        }
    }

    tsave = info->save_name;
    ttype = info->save_type;
    if( info->internal_type != WR_DONT_KNOW ) {
        info->save_type = info->internal_type;
    } else {
        info->save_type = info->file_type;
    }
    info->save_name = WRGetTempFileName( ext );
    if( info->save_name == NULL ) {
        info->save_name = tsave;
        info->save_type = ttype;
        return( FALSE );
    }

    switch( info->save_type ) {
    case WR_WIN_BITMAP:
        ret = WRSaveBitmapResource( info, FALSE );
        break;

    case WR_WIN_ICON:
        ret = WRSaveIconResource( info, FALSE );
        break;

    case WR_WIN_CURSOR:
        ret = WRSaveCursorResource( info, FALSE );
        break;

    case WR_WIN16M_RES:
    case WR_WIN16W_RES:
    case WR_WINNTM_RES:
    case WR_WINNTW_RES:
        ret = WRSaveResourceToRES( info, FALSE );
        break;

    case WR_WIN16_EXE:
    case WR_WIN16_DLL:
        ret = WRSaveResourceToWin16EXE( info, FALSE );
        break;

    case WR_WINNT_EXE:
    case WR_WINNT_DLL:
        ret = WRSaveResourceToWinNTEXE( info, FALSE );
        break;

    case WR_WIN_RC:
    case WR_WIN_RC_STR:
    case WR_WIN_RC_MENU:
    case WR_WIN_RC_ACCEL:
    case WR_WIN_RC_DLG:
        ret = FALSE;
        break;

    case WR_DONT_KNOW:
    case WR_INVALID_FILE:
    default:
        WRDisplayErrorMsg( WR_BADSAVETYPE );
        ret = FALSE;
        break;
    }

    if( ret ) {
        ret = WRRelinkInfo( info );
    }

    if( ret ) {
        ret = WRRenameFile( info->tmp_file, info->save_name );
        if( !ret ) {
            ret = WRDeleteFile( info->save_name );
        }
    }

    MemFree( info->save_name );
    info->save_name = tsave;
    info->save_type = ttype;

    return( ret );
}

int WRAPI WRSaveObjectAs( const char *file, WRFileType file_type, WRSaveIntoData *idata )
{
    WRInfo      *info;
    long        type;
    int         ok;

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
        if( type == (long)(pointer_int)RT_GROUP_ICON ) {
            ok = WREDoSaveImageAs( info, idata, TRUE );
        } else if( type == (long)(pointer_int)RT_GROUP_CURSOR ) {
            ok = WREDoSaveImageAs( info, idata, FALSE );
        } else {
            ok = WREDoSaveObjectAs( info, idata );
        }
        idata = idata->next;
    }

    if( ok ) {
        ok = WRSaveResource( info, TRUE );
    }

    if( info != NULL ) {
        info->save_name = NULL;
        WRCleanDir( info->dir );
        WRFreeWRInfo( info );
    }

    return( ok );
}

int WRAPI WRSaveObjectInto( const char *file, WRSaveIntoData *idata, int *dup )
{
    WRInfo      *info;
    char        *tmp_file;
    char        ext[_MAX_EXT];
    long        type;
    int         ok;

    info = NULL;
    tmp_file = NULL;

    ok = (file != NULL && idata != NULL && dup != NULL);

    if( ok ) {
        info = WRLoadResource( file, WR_DONT_KNOW );
        ok = (info != NULL);
    }

    if( ok ) {
        _splitpath( info->file_name, NULL, NULL, NULL, ext );
        ok = ((tmp_file = WRGetTempFileName( ext )) != NULL);
    }

    if( ok ) {
        info->save_type = info->file_type;
        info->save_name = tmp_file;
    }

    // loop thru all of the data
    while( ok && idata != NULL ) {
        type = WResIDToNum( idata->type );
        if( type == (long)(pointer_int)RT_GROUP_ICON ) {
            ok = WREDoSaveImageInto( info, idata, dup, TRUE );
        } else if( type == (long)(pointer_int)RT_GROUP_CURSOR ) {
            ok = WREDoSaveImageInto( info, idata, dup, FALSE );
        } else {
            ok = WREDoSaveObjectInto( info, idata, dup );
        }
        idata = idata->next;
    }

    if( ok ) {
        ok = WRSaveResource( info, TRUE );
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

int WRAPI WRFindAndSetData( WResDir dir, WResID *type, WResID *name,
                                WResLangType *lang, void *data )
{
    WResLangNode    *lnode;
    int             ok;

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
    int             ok;

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

int WREDoSaveObjectAs( WRInfo *info, WRSaveIntoData *idata )
{
    int           ok;

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

int WREDoSaveImageAs( WRInfo *info, WRSaveIntoData *idata, int is_icon )
{
    int                 ok;
    BYTE                *data;
    uint_32             size;
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

int WREDoSaveObjectInto( WRInfo *info, WRSaveIntoData *idata, int *dup )
{
    int ok;
    int replace_nixed;

    replace_nixed = FALSE;

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
        return( TRUE );
    }

    return( ok );
}

int WREDoSaveImageInto( WRInfo *info, WRSaveIntoData *idata, int *dup, int is_icon )
{
    BYTE                *data;
    uint_32             size;
    WResLangNode        *lnode;
    int                 replace_nixed;
    int                 ok;

    replace_nixed = FALSE;
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
        return( TRUE );
    }

    return( ok );
}

int WQueryReplaceObject( void )
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
        return( FALSE );
    }

    return( TRUE );
}

int WQueryMergeStrings( WResID *rname )
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
        return( FALSE );
    }

    return( TRUE );
}

// this function returns TRUE if the save into may continue
int WRTestReplace( WRInfo *info, WRSaveIntoData *idata )
{
    WResTypeNode    *tnode;
    WResResNode     *rnode;
    WResLangNode    *lnode;
    long            type;
    void            *data;
    int             size;
    int             strings;

    if( info == NULL || info->dir == NULL || idata == NULL || idata->type == NULL ||
        idata->name == NULL ) {
        return( FALSE );
    }

    type = WResIDToNum( idata->type );

    strings = (type == (long)(pointer_int)RT_STRING);

    tnode = WRFindTypeNodeFromWResID( info->dir, idata->type );
    if( tnode == NULL ) {
        return( TRUE );
    }

    rnode = WRFindResNodeFromWResID( tnode, idata->name );
    if( rnode == NULL ) {
        return( TRUE );
    }

    lnode = WRFindLangNodeFromLangType( rnode, &idata->lang );
    if( lnode == NULL ) {
        return( TRUE );
    }

    if( strings ) {
        if( !WQueryMergeStrings( idata->name ) ) {
            return( FALSE );
        }
        data = WRLoadResData( info->file_name, lnode->Info.Offset, lnode->Info.Length );
        size = lnode->Info.Length;
        if( !WRMergeStringData( &data, &size, idata->data, idata->size,
                                WRIs32Bit( info->save_type ), TRUE ) ) {
            if( data != NULL ) {
                MemFree( data );
            }
            return( FALSE );
        }
        if( idata->data != NULL ) {
            MemFree( idata->data );
        }
        idata->data = data;
        idata->size = size;
    } else {
        if( !WQueryReplaceObject() ) {
            return( FALSE );
        }
    }

    if( type == (long)(pointer_int)RT_GROUP_ICON || type == (long)(pointer_int)RT_GROUP_CURSOR ) {
        if( !WRDeleteGroupImages( info, lnode, type ) ) {
            return( FALSE );
        }
    }

    if( !WRRemoveLangNodeFromDir( info->dir, &tnode, &rnode, &lnode ) ) {
        return( FALSE );
    }

    return( TRUE );
}
