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


#include "guiwind.h"
#if defined( __WINDOWS__ ) && !defined( __WINDOWS_386__ )
    #pragma library( "commdlg.lib" );
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

#define HAVE_DRIVES

#include <direct.h>
#include <dos.h>

#include "guidlg.h"
#include "guifdlg.h"
#include "guixutil.h"
#include "guistr.h"
#include "ctl3dcvr.h"
#include "wprocmap.h"

#include "clibext.h"


#ifndef __OS2_PM__
static  char    *LastPath; // this is set in NT for the sake of viper
#endif

#if defined (__NT__)
/* Changed default from hook to not */
static  bool    hookFileDlg = false;
#else
static  bool    hookFileDlg = true;
#endif

extern UINT CALLBACK OpenHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

void GUIHookFileDlg( bool hook )
{
    hookFileDlg = hook;
}

#if defined( __OS2_PM__ )

int GUIGetFileName( gui_window *wnd, open_file_name *ofn )
{
  #ifdef _M_I86
    wnd = wnd;
    ofn = ofn;
  #else
    FILEDLG             fdlg;
    int                 str_index;
    int                 rc;
    ULONG               i;
    ULONG               slen, flen;
    unsigned            drive;
    unsigned            old_drive;
    unsigned            drives;
    char                initial_path[_MAX_PATH];
    char                old_path[_MAX_PATH];
    char                fname[_MAX_FNAME + _MAX_EXT];
    char                *cwd;

    old_path[0] = '\0';
    fname[0] = '\0';
    cwd = getcwd( initial_path, _MAX_PATH );
    if( cwd ) {
        _splitpath( cwd, NULL, old_path, NULL, NULL );
    }

    drive = 0;
    initial_path[0] = '\0';
    if( ofn->initial_dir != NULL && ofn->initial_dir[0] != '\0' ) {
        if( ofn->initial_dir[1] == ':' ) {
            drive = ofn->initial_dir[0];
        }
        _splitpath( ofn->initial_dir, NULL, initial_path, NULL, NULL );
    }

    memset( &fdlg, 0 , sizeof( fdlg ) );

    fdlg.cbSize = sizeof( fdlg );

    fdlg.fl = FDS_CENTER | FDS_ENABLEFILELB;
    if( ofn->flags & OFN_ISSAVE ) {
        fdlg.fl |= FDS_SAVEAS_DIALOG;
        fdlg.pszOKButton = (PSZ)LIT( FDlg_Save_Text );
    } else {
        fdlg.fl |= FDS_OPEN_DIALOG;
        fdlg.pszOKButton = (PSZ)LIT( FDlg_Open_Text );
    }
    if( ofn->flags & OFN_ALLOWMULTISELECT ) {
        fdlg.fl |= FDS_MULTIPLESEL;
    }

    fdlg.pszTitle = ofn->title;

    if( ofn->file_name ) {
        strncpy( fdlg.szFullFile, ofn->file_name, CCHMAXPATH );
        fdlg.szFullFile[CCHMAXPATH-1] = '\0';
    }

    if( ( !ofn->file_name || !*ofn->file_name ) && ofn->filter_index >= 0 ) {
        str_index = 0;
        for( i=0; ;i++ ) {
            if( ofn->filter_list[i] == '\0' ) {
                if( ofn->filter_list[i+1] == '\0' ) {
                    break;
                }
                str_index++;
                if( str_index == ofn->filter_index * 2 - 1 ) {
                    i++;
                    break;
                }
            }
        }
        if( ofn->filter_list[i] != '\0' ) {
            strncpy( fdlg.szFullFile, ofn->filter_list + i, CCHMAXPATH );
            fdlg.szFullFile[CCHMAXPATH-1] = '\0';
        }
    }

  #if defined( HAVE_DRIVES )
    if( drive ) {
        _dos_getdrive( &old_drive );
        _dos_setdrive( tolower( drive ) - 'a' + 1, &drives );
        if( *initial_path && *old_path ) {
            chdir( initial_path );
        }
    }
  #endif

    rc = (int)WinFileDlg( HWND_DESKTOP, GUIGetParentFrameHWND( wnd ), &fdlg );

    if( fdlg.papszFQFilename ) {
        ofn->file_name[0] = '\0';
        slen = 0;
        for( i=0; i<fdlg.ulFQFCount; i++ ) {
            flen = strlen( fdlg.papszFQFilename[0][i] );
            if( ( slen + flen + 2 ) > ofn->max_file_name ) {
                return( OFN_RC_FAILED_TO_INITIALIZE );
            }
            if( slen ) {
                ofn->file_name[slen++] = ' ';
            }
            memcpy( &ofn->file_name[slen], fdlg.papszFQFilename[0][i], flen + 1);
            slen += flen;
        }
        WinFreeFileDlgList( fdlg.papszFQFilename );
        if( ofn->base_file_name  != NULL ) {
            ofn->base_file_name[0] = '\0';
        }
    } else {
        if( strlen( fdlg.szFullFile ) > ( ofn->max_file_name - 1 ) ) {
            return( OFN_RC_FAILED_TO_INITIALIZE );
        } else {
            strcpy( ofn->file_name, fdlg.szFullFile );
            _splitpath( fdlg.szFullFile, NULL, NULL, fname, NULL );
            _splitpath( fdlg.szFullFile, NULL, NULL, NULL, fname+strlen(fname) );
        }
        if( ofn->base_file_name  != NULL ) {
            ofn->base_file_name[0] = '\0';
            if( strlen( fname ) <= ofn->max_base_file_name ) {
                strcpy( ofn->base_file_name, fname );
            }
        }
    }

  #if defined( HAVE_DRIVES )
    if( drive ) {
        _dos_setdrive( old_drive, &drives );
        if( *initial_path && *old_path ) {
            chdir( old_path );
        }
    }
  #endif

    if( fdlg.lReturn == DID_CANCEL ) {
        return( OFN_RC_NO_FILE_SELECTED );
    }

    if( rc ) {
        return( OFN_RC_FILE_SELECTED );
    }
  #endif
    return( OFN_RC_FAILED_TO_INITIALIZE );
} /* GUIGetFileName */

#else

#if defined(__NT__)
static char *GetStrFromEdit( HWND hDlg, gui_ctl_id id )
{
    char    *cp;
    LRESULT text_length;
    LRESULT text_copied;

    text_length = SendDlgItemMessage( hDlg, id, WM_GETTEXTLENGTH, 0, 0 );
    cp = (char *)GUIMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return ( NULL );
    }

    text_copied = SendDlgItemMessage( hDlg, id, WM_GETTEXT, text_length + 1, (LPARAM)(LPCSTR)cp );

    if( text_copied != text_length ) {
        // this is peculiar
        // do nothing for now
    }

    cp[text_length] = '\0';

    return( cp );
}
#endif

#ifdef __NT__
#define PATH_STATIC_CONTROL 1088
#endif

UINT CALLBACK OpenHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    UINT        ret;

    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;
    ret = false;
    switch( msg ) {
  #if defined(__NT__)
    case WM_DESTROY:
        LastPath = GetStrFromEdit( hwnd, PATH_STATIC_CONTROL );
        break;
  #endif
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug)
        CvrCtl3dSubclassDlgAll( hwnd );
        ret = true;
        break;
    }
    return( ret );
}

int GUIGetFileName( gui_window *wnd, open_file_name *ofn )
{
    OPENFILENAME        wofn;
    bool                issave;
    int                 rc;
    unsigned            drive;
#if defined(HAVE_DRIVES)
    unsigned            old_drive;
    unsigned            drives;
#endif

    LastPath = NULL;
    if( ofn->initial_dir != NULL && ofn->initial_dir[0] != '\0' && ofn->initial_dir[1] == ':' ) {
        drive = ofn->initial_dir[0];
        memmove( ofn->initial_dir, ofn->initial_dir+2, strlen( ofn->initial_dir+2 ) + 1 );
    } else {
        drive = 0;
    }

    memset( &wofn, 0 , sizeof( wofn ) );

    if( ofn->flags & OFN_ISSAVE ) {
        issave = true;
    } else {
        issave = false;
    }

    wofn.Flags = 0;
    if( hookFileDlg ) {
        wofn.Flags |= OFN_ENABLEHOOK;
    }
    if( !(ofn->flags & OFN_CHANGEDIR) ) {
        wofn.Flags |= OFN_NOCHANGEDIR;
    }

    if( ofn->flags & OFN_OVERWRITEPROMPT ) {
        wofn.Flags |= OFN_OVERWRITEPROMPT;
    }
    if( ofn->flags & OFN_HIDEREADONLY ) {
        wofn.Flags |= OFN_HIDEREADONLY;
    }
    if( ofn->flags & OFN_FILEMUSTEXIST ) {
        wofn.Flags |= OFN_FILEMUSTEXIST;
    }
    if( ofn->flags & OFN_PATHMUSTEXIST ) {
        wofn.Flags |= OFN_PATHMUSTEXIST;
    }
    if( ofn->flags & OFN_ALLOWMULTISELECT ) {
        wofn.Flags |= OFN_ALLOWMULTISELECT;
    }
    wofn.hwndOwner = GUIGetParentFrameHWND( wnd );
    wofn.hInstance = GUIMainHInst;
    wofn.lStructSize = sizeof( wofn );
    wofn.lpstrFilter = ofn->filter_list;
    wofn.nFilterIndex = ofn->filter_index;
    wofn.lpstrFile = ofn->file_name;
    wofn.nMaxFile = ofn->max_file_name;
    wofn.lpstrFileTitle = ofn->base_file_name;
    wofn.nMaxFileTitle = ofn->max_base_file_name;
    wofn.lpstrTitle = ofn->title;
    wofn.lpstrInitialDir = ofn->initial_dir;
    wofn.lpfnHook = (LPOFNHOOKPROC)NULL;
    if( hookFileDlg ) {
        wofn.lpfnHook = (LPOFNHOOKPROC)MakeOpenFileHookProcInstance( OpenHook, GUIMainHInst );
    }

#if defined( HAVE_DRIVES )
    if( drive ) {
        _dos_getdrive( &old_drive );
        _dos_setdrive( tolower( drive ) - 'a' + 1, &drives );
    }
#endif
    if( issave ) {
        rc = GetSaveFileName( &wofn );
    } else {
        rc = GetOpenFileName( &wofn );
    }

    if( hookFileDlg ) {
        (void)FreeProcInstance( (FARPROC)wofn.lpfnHook );
    }

    if( LastPath && ( !rc || !( ofn->flags & OFN_WANT_LAST_PATH ) ) ) {
        GUIMemFree( LastPath );
        LastPath = NULL;
    }
    ofn->last_path = LastPath;
#if defined( HAVE_DRIVES )
    if( drive ) {
        _dos_setdrive( old_drive, &drives );
    }
#endif
    if( rc ) {
        return( OFN_RC_FILE_SELECTED );
    }
    if( !CommDlgExtendedError() ) {
        return( OFN_RC_NO_FILE_SELECTED );
    }
    return( OFN_RC_FAILED_TO_INITIALIZE );
} /* _GUIGetFileName */

#endif
