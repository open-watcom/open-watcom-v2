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
#include <commdlg.h>
#include <dlgs.h>
#include <direct.h>
#include "winst.h"
#include "wmsg.h"
#include "ldstr.h"
#include "rcstr.grh"
#include "wctl3d.h"
#include "wstrdup.h"
#include "wgetfn.h"
#include "wclbproc.h"
#include "pathgrp2.h"

#include "clibext.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT UINT_PTR CALLBACK WOpenOFNHookProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    OPENFILE,
    SAVEFILE
} WGetFileAction;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char *WGetFileName( WGetFileStruct *, HWND, DWORD, WGetFileAction );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char  WFnTitle[_MAX_PATH];
static char  WFn[_MAX_PATH];
static char  WInitialDir[_MAX_PATH] = { 0 };
static int   WFileFilter = 1;

static char *WFindFileFilterFromIndex( char *filter, int index )
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
                if( (ind % 2) == 0 && (ind / 2) == index ) {
                    return( &filter[i + 1] );
                }
            }
        }
    }

    return( "*.*" );
}

int WGetFileFilter( void )
{
    return( WFileFilter );
}

void WSetFileFilter( int filter )
{
    WFileFilter = filter;
}

char *WGetInitialDir( void )
{
    if( *WInitialDir != '\0' ) {
        return( WInitialDir );
    }
    return( NULL );
}

void WSetInitialDir( char *dir )
{
    size_t len;

    if( dir != NULL && *dir != '\0' ) {
        len = strlen( dir );
        if( len >= _MAX_PATH ) {
            return;
        }
        strcpy( WInitialDir, dir );
    }
}

char *WGetOpenFileName( HWND parent, WGetFileStruct *gf )
{
    return( WGetFileName( gf, parent, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                      OFN_FILEMUSTEXIST, OPENFILE ) );
}

char *WGetSaveFileName( HWND parent, WGetFileStruct *gf )
{
    return( WGetFileName( gf, parent, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                      OFN_OVERWRITEPROMPT, SAVEFILE ) );
}

char *WGetFileName( WGetFileStruct *gf, HWND owner, DWORD flags, WGetFileAction action )
{
    OPENFILENAME    wofn;
    BOOL            ret;
    DWORD           error;
    size_t          len;
    pgroup2         pg;
    HINSTANCE       app_inst;

    if( gf == NULL ) {
        return ( NULL );
    }

    app_inst = WGetEditInstance();

    if( app_inst == NULL || owner == NULL ) {
        return( NULL );
    }

    if( gf->title != NULL ) {
        len = strlen( gf->title );
        if( len < _MAX_PATH ) {
            memcpy( WFnTitle, gf->title, len + 1 );
        } else {
            memcpy( WFnTitle, gf->title, _MAX_PATH );
            WFnTitle[_MAX_PATH - 1] = 0;
        }
    } else {
        WFnTitle[0] = '\0';
    }

    if( gf->file_name != NULL && *gf->file_name != '\0' ) {
        _splitpath2( gf->file_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( pg.drive[0] != '\0' || pg.dir[0] != '\0' ) {
            _makepath( WInitialDir, pg.drive, pg.dir, NULL, NULL );
        }
        _makepath( WFn, NULL, NULL, pg.fname, pg.ext );
    } else {
        WFn[0] = '\0';
    }

    /* set the initial directory */
    if( *WInitialDir == '\0' ) {
        getcwd( WInitialDir, _MAX_PATH );
        WFileFilter = 1;
    }

#if !defined( __NT__ )
    // CTL3D no longer requires this
    flags |= OFN_ENABLEHOOK;
#endif

    /* initialize the OPENFILENAME struct */
    memset( &wofn, 0, sizeof( OPENFILENAME ) );

    /* fill in non-variant fields of OPENFILENAME struct */
    wofn.lStructSize = sizeof( OPENFILENAME );
    wofn.hwndOwner = owner;
    wofn.hInstance = app_inst;
    wofn.lpstrFilter = gf->filter;
    wofn.lpstrCustomFilter = NULL;
    wofn.nMaxCustFilter = 0;
    wofn.nFilterIndex = WFileFilter;
    wofn.lpstrFile = WFn;
    wofn.nMaxFile = _MAX_PATH;
    wofn.lpstrFileTitle = WFnTitle;
    wofn.nMaxFileTitle = _MAX_PATH;
    wofn.lpstrInitialDir = WInitialDir;
    wofn.lpstrTitle = WFnTitle;
    wofn.Flags = flags;
#if !defined( __NT__ )
    wofn.lpfnHook = MakeProcInstance_OFNHOOK( WOpenOFNHookProc, app_inst );
#endif

#if 0
    wofn.nFileOffset = 0L;
    wofn.nFileExtension = 0L;
    wofn.lpstrDefExt = NULL;
    wofn.lCustData = NULL;
    wofn.lpfnHook = NULL;
    wofn.lpTemplateName = NULL;
#endif

    if( action == OPENFILE ) {
        ret = GetOpenFileName( (LPOPENFILENAME)&wofn );
    } else if( action == SAVEFILE ) {
        ret = GetSaveFileName( (LPOPENFILENAME)&wofn );
    } else {
        return( NULL );
    }

#ifndef __NT__
    FreeProcInstance_OFNHOOK( wofn.lpfnHook );
#endif

    if( !ret ) {
        error = CommDlgExtendedError();
        if( error != 0 ) {
            WDisplayErrorMsg( W_ERRORSELFILE );
        }
        return( NULL );
    } else {
        memcpy( WInitialDir, WFn, wofn.nFileOffset );
        if( WInitialDir[wofn.nFileOffset - 1] == '\\' &&
            WInitialDir[wofn.nFileOffset - 2] != ':' ) {
            WInitialDir[wofn.nFileOffset - 1] = '\0';
        } else {
            WInitialDir[wofn.nFileOffset] = '\0';
        }
        WFileFilter = wofn.nFilterIndex;
        _splitpath2( WFn, pg.buffer, NULL, NULL, NULL, &pg.ext );
        if( pg.ext[0] == '\0' ) {
            pg.ext = WFindFileFilterFromIndex( gf->filter, wofn.nFilterIndex ) + 1;
            if( pg.ext[1] != '*' ) {
                strcat( WFn, pg.ext );
            }
        }
    }

    UpdateWindow( owner );

    return( WStrDup( WFn ) );
}

#ifndef __NT__
WINEXPORT UINT_PTR CALLBACK WOpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char    *title;

    /* unused parameters */ (void)wparam; (void)lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug)
        //WCtl3dSubclassDlgAll( hwnd );

        title = AllocRCString( W_GETFNCOMBOTITLE );
        if( title != NULL ) {
            SendDlgItemMessage( hwnd, stc2, WM_SETTEXT, 0, (LPARAM)(LPCSTR)title );
            FreeRCString( title );
        }

        return( TRUE );
    }

    return( FALSE );
}
#endif

bool WGetInternalRESName( const char *filename, char *newname )
{
    pgroup2     pg;

    if( filename != NULL && newname != NULL ) {
        _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( newname, pg.drive, pg.dir, pg.fname, "res" );
        return( true );
    }

    return( false );
}

void WMassageFilter( char *filter )
{
    if( filter != NULL ) {
        for( ; *filter != '\0'; filter++ ) {
            if( *filter == '\t' ) {
                *filter = '\0';
            }
        }
    }
}
