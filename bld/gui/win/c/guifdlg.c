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


#if defined( __WINDOWS__ ) && defined( _M_I86 )
    #pragma library( "commdlg.lib" );
#endif

#include "guiwind.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <direct.h>
#include "guidlg.h"
#include "guifdlg.h"
#include "guixutil.h"
#include "guistr.h"
//#include "guixhook.h"
#include "ctl3dcvr.h"
#include "wclbproc.h"
#include "guixwind.h"
#include "pathgrp2.h"

#include "clibext.h"


/* Local Window callback functions prototypes */
#ifndef __OS2_PM__
WINEXPORT UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
#endif

#ifndef __OS2_PM__
static  char    *LastPath; // this is set in NT for the sake of viper
#endif

#if defined (__NT__)
/* Changed default from hook to not */
static  bool    hookFileDlg = false;
#else
static  bool    hookFileDlg = true;
#endif

void GUIAPI GUIHookFileDlg( bool hook )
{
    hookFileDlg = hook;
}

#if defined( __OS2_PM__ )

int GUIGetFileName( gui_window *wnd, open_file_name *ofn )
{
  #ifdef _M_I86
    /* unused parameters */ (void)wnd; (void)ofn;
  #else
    FILEDLG             fdlg;
    int                 str_index;
    int                 rc;
    int                 rc2;
    ULONG               i;
    ULONG               slen;
    ULONG               flen;
    int                 new_drive;
    int                 old_drive;
    pgroup2             pg1;
    pgroup2             pg2;
    char                *cwd;

    cwd = getcwd( pg2.buffer, sizeof( pg2.buffer ) );
    if( cwd != NULL ) {
        _splitpath2( cwd, pg1.buffer, NULL, &pg1.dir, NULL, NULL );
    } else {
        pg1.dir = "";
    }

    new_drive = 0;
    if( ofn->initial_dir != NULL && ofn->initial_dir[0] != '\0' ) {
        _splitpath2( ofn->initial_dir, pg2.buffer, &pg2.drive, &pg2.dir, NULL, NULL );
        if( pg2.drive[0] != '\0' && pg2.drive[1] == ':' ) {
            new_drive = tolower( (unsigned char)pg2.drive[0] ) - 'a' + 1;
        }
    } else {
        pg2.dir = "";
    }

    memset( &fdlg, 0 , sizeof( fdlg ) );

    fdlg.cbSize = sizeof( fdlg );

    fdlg.fl = FDS_CENTER | FDS_ENABLEFILELB;
    if( ofn->flags & FN_ISSAVE ) {
        fdlg.fl |= FDS_SAVEAS_DIALOG;
        fdlg.pszOKButton = (PSZ)LIT( FDlg_Save_Text );
    } else {
        fdlg.fl |= FDS_OPEN_DIALOG;
        fdlg.pszOKButton = (PSZ)LIT( FDlg_Open_Text );
    }
    if( ofn->flags & FN_ALLOWMULTISELECT ) {
        fdlg.fl |= FDS_MULTIPLESEL;
    }

    fdlg.pszTitle = (char *)ofn->title;

    if( ofn->file_name != NULL ) {
        strncpy( fdlg.szFullFile, ofn->file_name, CCHMAXPATH );
        fdlg.szFullFile[CCHMAXPATH - 1] = '\0';
    }

    if( ( ofn->file_name == NULL || *ofn->file_name == '\0' ) && ofn->filter_index >= 0 ) {
        str_index = 0;
        for( i = 0; ; i++ ) {
            if( ofn->filter_list[i] == '\0' ) {
                if( ofn->filter_list[i + 1] == '\0' ) {
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
            fdlg.szFullFile[CCHMAXPATH - 1] = '\0';
        }
    }

    old_drive = 0;
    if( new_drive ) {
        old_drive = _getdrive();
        _chdrive( new_drive );
        if( pg2.dir[0] != '\0' && pg1.dir[0] != '\0' ) {
            chdir( pg2.dir );
        }
    }

    rc = (int)WinFileDlg( HWND_DESKTOP, GUIGetParentFrameHWND( wnd ), &fdlg );

    if( new_drive ) {
        _chdrive( old_drive );
        if( pg2.dir[0] != '\0' && pg1.dir[0] != '\0' ) {
            chdir( pg1.dir );
        }
    }

    rc2 = -1;

    if( fdlg.papszFQFilename != NULL ) {
        if( ofn->file_name != NULL && ofn->max_file_name > 0 ) {
            ofn->file_name[0] = '\0';
            slen = 0;
            for( i = 0; i < fdlg.ulFQFCount; i++ ) {
                flen = strlen( fdlg.papszFQFilename[0][i] );
                if( ( slen + flen + 2 ) > ofn->max_file_name ) {
                    rc2 = FN_RC_FAILED_TO_INITIALIZE;
                    break;
                }
                if( slen ) {
                    ofn->file_name[slen++] = ' ';
                }
                memcpy( &ofn->file_name[slen], fdlg.papszFQFilename[0][i], flen + 1 );
                slen += flen;
            }
        }
        WinFreeFileDlgList( fdlg.papszFQFilename );
        if( ofn->base_file_name != NULL && ofn->max_base_file_name > 0 ) {
            ofn->base_file_name[0] = '\0';
        }
    } else {
        pg2.buffer[0] = '\0';
        if( ofn->file_name != NULL && ofn->max_file_name > 0 ) {
            ofn->file_name[0] = '\0';
            if( strlen( fdlg.szFullFile ) < ofn->max_file_name ) {
                strcpy( ofn->file_name, fdlg.szFullFile );
                _splitpath2( fdlg.szFullFile, pg1.buffer, NULL, NULL, &pg1.fname, &pg1.ext );
                _makepath( pg2.buffer, NULL, NULL, pg1.fname, pg1.ext );
            } else {
                rc2 = FN_RC_FAILED_TO_INITIALIZE;
            }
        }
        if( ofn->base_file_name != NULL && ofn->max_base_file_name > 0 ) {
            ofn->base_file_name[0] = '\0';
            if( strlen( pg2.buffer ) < ofn->max_base_file_name ) {
                strcpy( ofn->base_file_name, pg2.buffer );
            } else {
                rc2 = FN_RC_FAILED_TO_INITIALIZE;
            }
        }
    }

    if( rc2 != -1 ) {
        return( rc2 );
    }

    if( fdlg.lReturn == DID_CANCEL ) {
        return( FN_RC_NO_FILE_SELECTED );
    }

    if( rc ) {
        return( FN_RC_FILE_SELECTED );
    }
  #endif
    return( FN_RC_FAILED_TO_INITIALIZE );
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

UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
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
    int                 new_drive;
    int                 old_drive;

    LastPath = NULL;
    new_drive = 0;
    if( ofn->initial_dir != NULL && ofn->initial_dir[0] != '\0' && ofn->initial_dir[1] == ':' ) {
        new_drive = tolower( (unsigned char)ofn->initial_dir[0] ) - 'a' + 1;
        memmove( ofn->initial_dir, ofn->initial_dir + 2, strlen( ofn->initial_dir + 2 ) + 1 );
    }

    memset( &wofn, 0 , sizeof( wofn ) );

    if( ofn->flags & FN_ISSAVE ) {
        issave = true;
    } else {
        issave = false;
    }

    wofn.Flags = 0;
    if( hookFileDlg ) {
        wofn.Flags |= OFN_ENABLEHOOK;
    }
    if( (ofn->flags & FN_CHANGEDIR) == 0 ) {
        wofn.Flags |= OFN_NOCHANGEDIR;
    }

    if( ofn->flags & FN_OVERWRITEPROMPT ) {
        wofn.Flags |= OFN_OVERWRITEPROMPT;
    }
    if( ofn->flags & FN_HIDEREADONLY ) {
        wofn.Flags |= OFN_HIDEREADONLY;
    }
    if( ofn->flags & FN_FILEMUSTEXIST ) {
        wofn.Flags |= OFN_FILEMUSTEXIST;
    }
    if( ofn->flags & FN_PATHMUSTEXIST ) {
        wofn.Flags |= OFN_PATHMUSTEXIST;
    }
    if( ofn->flags & FN_ALLOWMULTISELECT ) {
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
        wofn.lpfnHook = MakeProcInstance_OFNHOOK( OpenOFNHookProc, GUIMainHInst );
    }

    old_drive = 0;
    if( new_drive ) {
        old_drive = _getdrive();
        _chdrive( new_drive );
    }
    if( issave ) {
        rc = GetSaveFileName( &wofn );
    } else {
        rc = GetOpenFileName( &wofn );
    }

    if( hookFileDlg ) {
        FreeProcInstance_OFNHOOK( wofn.lpfnHook );
    }

    if( LastPath && ( rc == 0 || (ofn->flags & FN_WANT_LAST_PATH) == 0 ) ) {
        GUIMemFree( LastPath );
        LastPath = NULL;
    }
    ofn->last_path = LastPath;
    if( new_drive ) {
        _chdrive( old_drive );
    }
    if( rc ) {
        return( FN_RC_FILE_SELECTED );
    }
    if( !CommDlgExtendedError() ) {
        return( FN_RC_NO_FILE_SELECTED );
    }
    return( FN_RC_FAILED_TO_INITIALIZE );
} /* _GUIGetFileName */

#endif
