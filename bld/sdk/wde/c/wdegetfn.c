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
#include <commdlg.h>
#include <dlgs.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>

#include "wdeglbl.h"
#include "wdedebug.h"
#include "wdemain.h"
#include "wdemsgbx.h"
#include "wdemsgs.h"
#include "wdemem.h"
#include "wdesdup.h"
#include "wdectl3d.h"
#include "wdegetfn.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern UINT WINEXPORT WdeOpenHookProc ( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WDEOPENFILE
,   WDESAVEFILE
} WdeGetFileNameAction;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char *WdeGetFileName ( WdeGetFileStruct *, DWORD,
                              WdeGetFileNameAction );

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
    for( i=0; ; i++ ) {
        if( filter[i] == '\0' ) {
            if( filter[i+1] == '\0' ) {
                break;
            }
            index ++;
            if( ( index % 2 ) == 0 ) {
                if( !stricmp( last_filter, &filter[i+1] ) ) {
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
        for( i=0; ; i++ ) {
            if( filter[i] == '\0' ) {
                if( filter[i+1] == '\0' ) {
                    break;
                }
                ind++;
                if( ( ind % 2 == 0 ) && ( ind / 2 == index ) ) {
                    return( &filter[i+1] );
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
        WdeMemFree( WdeFileFilter );
    }
}

void WdeSetFileFilter( char *filter )
{
    WdeFreeFileFilter();
    WdeFileFilter = WdeStrDup( filter );
}

char *WdeGetInitialDir ( void )
{
    if ( *wde_initial_dir ) {
        return ( wde_initial_dir );
    }
    return ( NULL );
}

void WdeSetInitialDir ( char *dir )
{
    int len;

    if ( dir && *dir ) {
        len = strlen ( dir );
        if ( len >= MAXFILENAME ) {
            return;
        }
        strcpy ( wde_initial_dir, dir );
    }

}

char *WdeGetOpenFileName ( WdeGetFileStruct *gf )
{
    return ( WdeGetFileName ( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                  OFN_FILEMUSTEXIST,
                              WDEOPENFILE ) );
}

char *WdeGetSaveFileName ( WdeGetFileStruct *gf )
{
    return ( WdeGetFileName ( gf, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                  OFN_OVERWRITEPROMPT,
                              WDESAVEFILE ) );
}

char *WdeGetFileName( WdeGetFileStruct *gf, DWORD flags,
                      WdeGetFileNameAction action )
{
    OPENFILENAME        wdeofn;
    HWND                owner_window;
    DWORD               error;
    char                fn_drive[_MAX_DRIVE];
    char                fn_dir[_MAX_DIR];
    char                fn_name[_MAX_FNAME];
    char                fn_ext[_MAX_EXT];
    char                ext[_MAX_EXT+1];
    HINSTANCE           app_inst;
    int                 len;
    int                 filter;
    Bool                ret;

    if( !gf ) {
        return( NULL );
    }

    owner_window = WdeGetMainWindowHandle();
    app_inst = WdeGetAppInstance();

    if( !app_inst || !owner_window ) {
        return( NULL );
    }

    /* set the initial directory */
    if( !*wde_initial_dir ) {
        getcwd( wde_initial_dir, MAXFILENAME );
    }

    if( gf->title ) {
        len = strlen( gf->title );
        if ( len < MAX_NAME ) {
            memcpy( wdefntitle, gf->title, len + 1 );
        } else {
            memcpy( wdefntitle, gf->title, MAX_NAME );
            wdefntitle[MAX_NAME-1] = 0;
        }
    } else {
        wdefntitle[0] = 0;
    }

    filter = 0;

    if( gf->file_name && *gf->file_name ) {
        _splitpath( gf->file_name, fn_drive, fn_dir, fn_name, fn_ext );
        if( *fn_drive || *fn_dir ) {
            _makepath( wde_initial_dir, fn_drive, fn_dir, NULL, NULL );
        }
        _makepath( wde_file_name, NULL, NULL, fn_name, fn_ext );
        if( fn_ext[0] ) {
            ext[0] = '*';
            strcpy( ext+1, fn_ext );
            filter = WdeFindFileFilterIndex( gf->filter, ext );
        }
    } else {
        wde_file_name[0] = 0;
    }

    if( !filter ) {
        filter = max( 1, WdeFindFileFilterIndex( gf->filter, WdeFileFilter ) );
    }

    //ctl3d no longer requires this
    flags |= OFN_ENABLEHOOK;

    /* initialize the OPENFILENAME struct */
    memset (&wdeofn, 0, sizeof(OPENFILENAME));

    /* fill in non-variant fields of OPENFILENAME struct */
    wdeofn.lStructSize       = sizeof(OPENFILENAME);
    wdeofn.hwndOwner         = owner_window;
    wdeofn.hInstance         = app_inst;
    wdeofn.lpstrFilter       = gf->filter;
    wdeofn.lpstrCustomFilter = NULL;
    wdeofn.nMaxCustFilter    = 0;
    wdeofn.nFilterIndex      = filter;
    wdeofn.lpstrFile         = wde_file_name;
    wdeofn.nMaxFile          = MAXFILENAME;
    wdeofn.lpstrFileTitle    = wdefntitle;
    wdeofn.nMaxFileTitle     = MAXFILENAME;
    wdeofn.lpstrInitialDir   = wde_initial_dir;
    wdeofn.lpstrTitle        = wdefntitle;
    wdeofn.Flags             = flags;
    wdeofn.lpfnHook          = (LPVOID) MakeProcInstance
                                ( (LPVOID) WdeOpenHookProc, app_inst );

#if 0
    wdeofn.nFileOffset       = 0L;
    wdeofn.nFileExtension    = 0L;
    wdeofn.lpstrDefExt       = NULL;
    wdeofn.lCustData         = NULL;
    wdeofn.lpfnHook          = NULL;
    wdeofn.lpTemplateName    = NULL;
#endif

    if ( action == WDEOPENFILE ) {
        ret = GetOpenFileName ((LPOPENFILENAME)&wdeofn);
    } else if ( action == WDESAVEFILE ) {
        ret = GetSaveFileName ((LPOPENFILENAME)&wdeofn);
    } else {
        return ( NULL );
    }

    if ( wdeofn.lpfnHook ) {
        #ifndef __NT__
            FreeProcInstance ( (FARPROC) wdeofn.lpfnHook );
        #endif
    }

    gf->fn_offset = wdeofn.nFileOffset;

    gf->ext_offset = wdeofn.nFileExtension;

    /* show the dialog box */
    if( !ret ) {
        error = CommDlgExtendedError();
        if( error ) {
            WdeDisplayErrorMsg( WDE_ERRORSELECTINGFILE );
        }
        return( NULL );
    } else {
        memcpy( wde_initial_dir, wde_file_name, wdeofn.nFileOffset );
        if( ( wde_initial_dir[wdeofn.nFileOffset-1] == '\\' ) &&
            ( wde_initial_dir[wdeofn.nFileOffset-2] != ':' ) ) {
            wde_initial_dir[wdeofn.nFileOffset-1] = '\0';
        } else {
            wde_initial_dir[wdeofn.nFileOffset] = '\0';
        }
        _splitpath( wde_file_name, NULL, NULL, NULL, (fn_ext+1) );
        if( fn_ext[1] ) {
            fn_ext[0] = '*';
            WdeSetFileFilter( fn_ext );
        } else {
            char *out_ext;
            out_ext = WdeFindFileFilterFromIndex( gf->filter,
                                                  wdeofn.nFilterIndex );
            if( out_ext[2] != '*' ) {
                strcat( wde_file_name, &out_ext[1] );
            }
        }
    }

    UpdateWindow( WdeGetMainWindowHandle() );

    return( WdeStrDup( wde_file_name ) );
}

UINT WINEXPORT WdeOpenHookProc( HWND hwnd, UINT msg,
                                WPARAM wparam, LPARAM lparam )
{
    _wde_touch(wparam);
    _wde_touch(lparam);

    switch( msg ) {
        case WM_INITDIALOG:
            // We must call this to subclass the directory listbox even
            // if the app calls Ctl3dAutoSubclass (commdlg bug)
            // WdeCtl3dSubclassDlg ( hwnd, CTL3D_ALL );
            {
                char    *title;

                title = WdeAllocRCString( WDE_GETFNCOMBOTITLE );
                if( title ) {
                    SendDlgItemMessage( hwnd, stc2, WM_SETTEXT, 0, (LPARAM)title );
                    WdeFreeRCString( title );
                }
            }


            return( TRUE );
    }

    return ( FALSE );
}

