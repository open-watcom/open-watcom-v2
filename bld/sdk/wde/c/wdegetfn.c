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


#include "wdeglbl.h"
#include <commdlg.h>
#include <dlgs.h>
#include <direct.h>
#include "wdedebug.h"
#include "wdemain.h"
#include "wdemsgbx.h"
#include "rcstr.grh"
#include "wdesdup.h"
#include "wdectl3d.h"
#include "wdegetfn.h"
#include "wrdll.h"
#include "wclbproc.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT UINT_PTR CALLBACK WdeOpenOFNHookProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WDEOPENFILE,
    WDESAVEFILE
} WdeGetFileNameAction;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char *WdeGetFileName( WdeGetFileStruct *, DWORD, WdeGetFileNameAction );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
/* struct  passed to GetOpenFileName */
static char  wdefntitle[MAX_NAME];
static char  wde_file_name[MAXFILENAME];
static char  wde_initial_dir[MAXFILENAME] = { 0 };
static char  *WdeFileFilter = NULL;

static int WdeFindFileFilterIndex( char *filter, char *last_filter )
{
    int index;
    int i;

    if( filter == NULL || last_filter == NULL ) {
        return( 0 );
    }

    index = 1;
    for( i = 0;; i++ ) {
        if( filter[i] == '\0' ) {
            if( filter[i + 1] == '\0' ) {
                break;
            }
            index++;
            if( (index % 2) == 0 ) {
                if( stricmp( last_filter, &filter[i + 1] ) == 0 ) {
                    return( index / 2 );
                }
            }
        }
    }

    return( 0 );
}

static char *WdeFindFileFilterFromIndex( char *filter, int index )
{
    int ind;
    int i;

    if( filter != NULL ) {
        ind = 1;
        for( i = 0;; i++ ) {
            if( filter[i] == '\0' ) {
                if( filter[i + 1] == '\0' ) {
                    break;
                }
                ind++;
                if( (ind % 2) == 0 && ind / 2 == index ) {
                    return( &filter[i + 1] );
                }
            }
        }
    }

    return( "*.*" );
}

char *WdeGetFileFilter( void )
{
    return( WdeFileFilter );
}

void WdeFreeFileFilter( void )
{
    if( WdeFileFilter != NULL ) {
        WRMemFree( WdeFileFilter );
    }
}

void WdeSetFileFilter( char *filter )
{
    WdeFreeFileFilter();
    WdeFileFilter = WdeStrDup( filter );
}

char *WdeGetInitialDir( void )
{
    if( *wde_initial_dir != '\0' ) {
        return( wde_initial_dir );
    }
    return( NULL );
}

void WdeSetInitialDir( char *dir )
{
    int len;

    if( dir != NULL && *dir != '\0' ) {
        len = strlen( dir );
        if( len >= MAXFILENAME ) {
            return;
        }
        strcpy( wde_initial_dir, dir );
    }
}

char *WdeGetOpenFileName( WdeGetFileStruct *gf )
{
    return( WdeGetFileName( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                OFN_FILEMUSTEXIST,
                            WDEOPENFILE ) );
}

char *WdeGetSaveFileName( WdeGetFileStruct *gf )
{
    return( WdeGetFileName( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                OFN_OVERWRITEPROMPT,
                            WDESAVEFILE ) );
}

char *WdeGetFileName( WdeGetFileStruct *gf, DWORD flags, WdeGetFileNameAction action )
{
    OPENFILENAME        wdeofn;
    HWND                owner_window;
    DWORD               error;
    pgroup2             pg;
    char                ext[_MAX_EXT + 1];
    HINSTANCE           app_inst;
    int                 len;
    int                 filter;
    bool                ok;

    if( gf == NULL ) {
        return( NULL );
    }

    owner_window = WdeGetMainWindowHandle();
    app_inst = WdeGetAppInstance();

    if( app_inst == NULL || owner_window == NULL ) {
        return( NULL );
    }

    /* set the initial directory */
    if( *wde_initial_dir == '\0' ) {
        getcwd( wde_initial_dir, MAXFILENAME );
    }

    if( gf->title != NULL ) {
        len = strlen( gf->title );
        if ( len < MAX_NAME ) {
            memcpy( wdefntitle, gf->title, len + 1 );
        } else {
            memcpy( wdefntitle, gf->title, MAX_NAME );
            wdefntitle[MAX_NAME - 1] = 0;
        }
    } else {
        wdefntitle[0] = '\0';
    }

    filter = 0;

    if( gf->file_name != NULL && *gf->file_name != '\0' ) {
        _splitpath2( gf->file_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( pg.drive[0] != '\0' || pg.dir[0] != '\0' ) {
            _makepath( wde_initial_dir, pg.drive, pg.dir, NULL, NULL );
        }
        _makepath( wde_file_name, NULL, NULL, pg.fname, pg.ext );
        if( pg.ext[0] != '\0' ) {
            ext[0] = '*';
            strcpy( ext + 1, pg.ext );
            filter = WdeFindFileFilterIndex( gf->filter, ext );
        }
    } else {
        wde_file_name[0] = '\0';
    }

    if( filter == 0 ) {
        filter = WdeFindFileFilterIndex( gf->filter, WdeFileFilter );
        if( filter < 1 ) {
            filter = 1;
        }
    }

    // CTL3D no longer requires this
#if !defined( __NT__ )
    flags |= OFN_ENABLEHOOK;
#endif
    /* initialize the OPENFILENAME struct */
    memset( &wdeofn, 0, sizeof( OPENFILENAME ) );

    /* fill in non-variant fields of OPENFILENAME struct */
    wdeofn.lStructSize = sizeof( OPENFILENAME );
    wdeofn.hwndOwner = owner_window;
    wdeofn.hInstance = app_inst;
    wdeofn.lpstrFilter = gf->filter;
    wdeofn.lpstrCustomFilter = NULL;
    wdeofn.nMaxCustFilter = 0;
    wdeofn.nFilterIndex = filter;
    wdeofn.lpstrFile = wde_file_name;
    wdeofn.nMaxFile = MAXFILENAME;
    wdeofn.lpstrFileTitle = wdefntitle;
    wdeofn.nMaxFileTitle = MAXFILENAME;
    wdeofn.lpstrInitialDir = wde_initial_dir;
    wdeofn.lpstrTitle = wdefntitle;
    wdeofn.Flags = flags;
#if !defined( __NT__ )
    wdeofn.lpfnHook = MakeProcInstance_OFNHOOK( WdeOpenOFNHookProc, app_inst );
#endif

#if 0
    wdeofn.nFileOffset = 0L;
    wdeofn.nFileExtension = 0L;
    wdeofn.lpstrDefExt = NULL;
    wdeofn.lCustData = NULL;
    wdeofn.lpfnHook = NULL;
    wdeofn.lpTemplateName = NULL;
#endif

    if( action == WDEOPENFILE ) {
        ok = ( GetOpenFileName( (LPOPENFILENAME)&wdeofn ) != 0 );
    } else if( action == WDESAVEFILE ) {
        ok = ( GetSaveFileName( (LPOPENFILENAME)&wdeofn ) != 0 );
    } else {
        return( NULL );
    }

#ifndef __NT__
    FreeProcInstance_OFNHOOK( wdeofn.lpfnHook );
#endif

    gf->fn_offset = wdeofn.nFileOffset;
    gf->ext_offset = wdeofn.nFileExtension;

    /* show the dialog box */
    if( !ok ) {
        error = CommDlgExtendedError();
        if( error ) {
            WdeDisplayErrorMsg( WDE_ERRORSELECTINGFILE );
        }
        return( NULL );
    } else {
        memcpy( wde_initial_dir, wde_file_name, wdeofn.nFileOffset );
        if( wde_initial_dir[wdeofn.nFileOffset - 1] == '\\' &&
            wde_initial_dir[wdeofn.nFileOffset - 2] != ':' ) {
            wde_initial_dir[wdeofn.nFileOffset - 1] = '\0';
        } else {
            wde_initial_dir[wdeofn.nFileOffset] = '\0';
        }
        _splitpath2( wde_file_name, pg.buffer, NULL, NULL, NULL, &pg.ext );
        if( pg.ext[0] != '\0' ) {
            ext[0] = '*';
            strcpy( ext + 1, pg.ext );
            WdeSetFileFilter( ext );
        } else {
            pg.ext = WdeFindFileFilterFromIndex( gf->filter, wdeofn.nFilterIndex ) + 1;
            if( pg.ext[1] != '*' ) {
                strcat( wde_file_name, pg.ext );
            }
        }
    }

    UpdateWindow( WdeGetMainWindowHandle() );

    return( WdeStrDup( wde_file_name ) );
}

UINT_PTR CALLBACK WdeOpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char    *title;

    _wde_touch( wparam );
    _wde_touch( lparam );

    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug)
        // WdeCtl3dSubclassDlgAll( hwnd );
        title = WdeAllocRCString( WDE_GETFNCOMBOTITLE );
        if( title != NULL ) {
            SendDlgItemMessage( hwnd, stc2, WM_SETTEXT, 0, (LPARAM)(LPCSTR)title );
            WdeFreeRCString( title );
        }
        return( TRUE );
    }

    return( FALSE );
}
