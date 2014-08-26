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
#include <string.h>
#include <stdlib.h>
#include <commdlg.h>
#include <dlgs.h>
#include <direct.h>
#include "wreglbl.h"
#include "wremain.h"
#include "wremsg.h"
#include "ldstr.h"
#include "rcstr.gh"
#include "wrestrdp.h"
#include "wrectl3d.h"
#include "wregetfn.h"
#include "wrdll.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT UINT CALLBACK WREOpenHookProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    OPENFILE,
    SAVEFILE
} WREGetFileAction;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char *WREGetFileName( WREGetFileStruct *, DWORD, WREGetFileAction );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char wrefntitle[_MAX_PATH];
static char wre_file_name[_MAX_PATH];
static char wre_initial_dir[_MAX_PATH] = { 0 };
static char *LastFileFilter = NULL;

static int WREFindFileFilterIndex( char *filter )
{
    int index;
    int i;

    if( filter != NULL && LastFileFilter != NULL ) {
        index = 1;
        for( i = 0;; i++ ) {
            if( filter[i] == '\0' ) {
                if( filter[i + 1] == '\0' ) {
                    break;
                }
                index++;
                if( index % 2 == 0 ) {
                    if( !stricmp( LastFileFilter, &filter[i + 1] ) ) {
                        return( index / 2 );
                    }
                }
            }
        }
    }

    return( 1 );
}

static char *WREFindFileFilterFromIndex( char *filter, int index )
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
                if( ind % 2 == 0 && ind / 2 == index ) {
                    return( &filter[i + 1] );
                }
            }
        }
    }

    return( "*.*" );
}

char *WREGetFileFilter( void )
{
    return( LastFileFilter );
}

void WRESetFileFilter( char *filter )
{
    WREFreeFileFilter();
    LastFileFilter = WREStrDup( filter );
}

void WREFreeFileFilter( void )
{
    if( LastFileFilter != NULL ) {
        WRMemFree( LastFileFilter );
    }
}

char *WREGetInitialDir( void )
{
    if( *wre_initial_dir != '\0' ) {
        return( wre_initial_dir );
    }
    return( NULL );
}

void WRESetInitialDir( char *dir )
{
    int len;

    if( dir != NULL && *dir != '\0' ) {
        len = strlen( dir );
        if( len >= _MAX_PATH ) {
            return;
        }
        strcpy( wre_initial_dir, dir );
    }

}

char *WREGetOpenFileName( WREGetFileStruct *gf )
{
    return( WREGetFileName( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                OFN_FILEMUSTEXIST,
                            OPENFILE ) );
}

char *WREGetSaveFileName( WREGetFileStruct *gf )
{
    return( WREGetFileName( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                OFN_OVERWRITEPROMPT,
                            SAVEFILE ) );
}

char *WREGetFileName( WREGetFileStruct *gf, DWORD flags, WREGetFileAction action )
{
    OPENFILENAME  wreofn;
    HWND          owner_window;
    bool          ret;
    DWORD         error;
    int           len;
    char          fn_drive[_MAX_DRIVE];
    char          fn_dir[_MAX_DIR];
    char          fn_name[_MAX_FNAME];
    char          fn_ext[_MAX_EXT + 1];
    HINSTANCE     app_inst;

    if( gf == NULL ) {
        return( NULL );
    }

    owner_window = WREGetMainWindowHandle();
    app_inst = WREGetAppInstance();

    if( app_inst == NULL || owner_window == NULL ) {
        return( NULL );
    }

    if( gf->title != NULL ) {
        len = strlen( gf->title );
        if( len < _MAX_PATH ) {
            memcpy( wrefntitle, gf->title, len + 1 );
        } else {
            memcpy( wrefntitle, gf->title, _MAX_PATH );
            wrefntitle[_MAX_PATH - 1] = 0;
        }
    } else {
        wrefntitle[0] = 0;
    }

    if( gf->file_name != NULL && *gf->file_name != '\0' ) {
        _splitpath( gf->file_name, fn_drive, fn_dir, fn_name, fn_ext );
        if( *fn_drive != '\0' || *fn_dir != '\0' ) {
            _makepath( wre_initial_dir, fn_drive, fn_dir, NULL, NULL );
        }
        _makepath( wre_file_name, NULL, NULL, fn_name, fn_ext );
    } else {
        wre_file_name[0] = 0;
    }

    /* set the initial directory */
    if( *wre_initial_dir == '\0' ) {
        getcwd( wre_initial_dir, _MAX_PATH );
    }

#if !defined ( __NT__ )
    // CTL3D no longer requires this
    flags |= OFN_ENABLEHOOK;
#endif

    /* initialize the OPENFILENAME struct */
    memset( &wreofn, 0, sizeof( OPENFILENAME ) );

    /* fill in non-variant fields of OPENFILENAME struct */
    wreofn.lStructSize = sizeof( OPENFILENAME );
    wreofn.hwndOwner = owner_window;
    wreofn.hInstance = app_inst;
    wreofn.lpstrFilter = gf->filter;
    wreofn.lpstrCustomFilter = NULL;
    wreofn.nMaxCustFilter = 0;
    wreofn.nFilterIndex = WREFindFileFilterIndex( gf->filter );
    wreofn.lpstrFile = wre_file_name;
    wreofn.nMaxFile = _MAX_PATH;
    wreofn.lpstrFileTitle = wrefntitle;
    wreofn.nMaxFileTitle = _MAX_PATH;
    wreofn.lpstrInitialDir = wre_initial_dir;
    wreofn.lpstrTitle = wrefntitle;
    wreofn.Flags = flags;
    wreofn.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)WREOpenHookProc, app_inst );

#if 0
    wreofn.nFileOffset = 0L;
    wreofn.nFileExtension = 0L;
    wreofn.lpstrDefExt = NULL;
    wreofn.lCustData = NULL;
    wreofn.lpTemplateName = NULL;
#endif

    if( action == OPENFILE ) {
        ret = GetOpenFileName( (LPOPENFILENAME)&wreofn ) != 0;
    } else if( action == SAVEFILE ) {
        ret = GetSaveFileName( (LPOPENFILENAME)&wreofn ) != 0;
    } else {
        return( NULL );
    }

#ifndef __NT__
    if( wreofn.lpfnHook != NULL ) {
        FreeProcInstance( (FARPROC)wreofn.lpfnHook );
    }
#endif

    if( !ret ) {
        error = CommDlgExtendedError();
        if( error ) {
            WREDisplayErrorMsg( WRE_ERRORSELECTINGFILE );
        }
        return( NULL );
    } else {
        memcpy( wre_initial_dir, wre_file_name, wreofn.nFileOffset );
        if( wre_initial_dir[wreofn.nFileOffset - 1] == '\\' &&
            wre_initial_dir[wreofn.nFileOffset - 2] != ':' ) {
            wre_initial_dir[wreofn.nFileOffset - 1] = '\0';
        } else {
            wre_initial_dir[wreofn.nFileOffset] = '\0';
        }
        if( gf->save_ext ) {
            _splitpath( wre_file_name, NULL, NULL, NULL, fn_ext + 1 );
            if( fn_ext[1] != '\0' ) {
                fn_ext[0] = '*';
                WRESetFileFilter( fn_ext );
            } else {
                char *out_ext;
                out_ext = WREFindFileFilterFromIndex( gf->filter, wreofn.nFilterIndex );
                if( out_ext[2] != '*' ) {
                    strcat( wre_file_name, &out_ext[1] );
                }
            }
        }
    }

    UpdateWindow( WREGetMainWindowHandle() );

    return( WREStrDup( wre_file_name ) );
}

UINT CALLBACK WREOpenHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char    *title;

    _wre_touch( wparam );
    _wre_touch( lparam );

    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug)
        //WRECtl3dSubclassDlgAll( hwnd );

        title = AllocRCString( WRE_GETFNCOMBOTITLE );
        if( title != NULL ) {
            SendDlgItemMessage( hwnd, stc2, WM_SETTEXT, 0, (LPARAM)title );
            FreeRCString( title );
        }
        return( TRUE );
    }

    return( FALSE );
}
