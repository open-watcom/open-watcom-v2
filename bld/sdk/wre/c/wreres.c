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


#include "commonui.h"
#include <stdio.h>
#include <string.h>
#include <ddeml.h>

#include "watcom.h"
#include "waccel.h"
#include "wmenu.h"
#include "wstring.h"

#include "wreglbl.h"
#include "wreres.h"
#include "wregetfn.h"
#include "wrestrdp.h"
#include "wremain.h"
#include "wrewait.h"
#include "wremsg.h"
#include "ldstr.h"
#include "rcstr.gh"
#include "wreselft.h"
#include "wrenames.h"
#include "wrectl3d.h"
#include "wrelist.h"
#include "wregcres.h"
#include "wredde.h"
#include "wreaccel.h"
#include "wremenu.h"
#include "wrestr.h"
#include "wredlg.h"
#include "wreimg.h"
#include "wreimage.h"
#include "wreftype.h"
#include "wresym.h"
#include "wre_rc.h"
#include "wrdll.h"
#include "jdlg.h"
#include "wreres.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define GB_PAD 9
#define LB_PAD 7

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WREResWndProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT LRESULT CALLBACK WREResInfoProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static int          WREIncNumRes( void );
static int          WREDecNumRes( void );
static bool         WREIsCurrentMDIWindowZoomed( void );
static WREResInfo   *WRELoadResource( const char * );
static void         WREActivateResourceWindow( WREResInfo *, WPARAM, LPARAM );
static bool         WREQuerySaveResOnDeleteRes( WREResInfo *, bool );
static bool         WREQuerySaveSymOnDeleteRes( WREResInfo *, bool );
static bool         WRESaveResourceToFile( WREResInfo * );
static bool         WRECreateResourceWindow( WREResInfo * );
static bool         WREDestroyResourceWindow( WREResInfo * );
static bool         WRECreateResInfoWindow( WREResInfo * );

bool    WRERemoveResource( WREResInfo * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
char    *WREResUntitled         = NULL;
char    *WREResOpenTitle        = NULL;
char    *WREResSaveTitle        = NULL;
char    *WREResSaveIntoTitle    = NULL;
char    *WREResSaveAsTitle      = NULL;
char    *WREResFilter           = NULL;
char    *WREResSaveMltFilter    = NULL;
char    *WRESymSaveFilter       = NULL;
char    *WRESymLoadTitle        = NULL;
char    *WRESymSaveTitle        = NULL;

char    *WREAccelFilter         = NULL;
char    *WREMenuFilter          = NULL;
char    *WREStringFilter        = NULL;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char             WREResClass[]           = "WREResClass";
static LIST             *WREResList             = NULL;
static unsigned         WREResCounter           = 0;
static int              WRENumRes               = 0;
static WREResInfo       *WRECurrentRes          = NULL;
static DLGPROC          WREResInfoWinProc       = NULL;
static HINSTANCE        WREAppInst              = NULL;
static HBRUSH           WREResInfoBrush         = NULL;

extern bool WRENoInterface;

static void WREMassageFilter( char *filter )
{
    WRMassageFilter( filter );
}

static void WREFiniStaticVars( void )
{
    if( WREResUntitled != NULL ) {
        FreeRCString( WREResUntitled );
        WREResUntitled = NULL;
    }
    if( WREResOpenTitle != NULL ) {
        FreeRCString( WREResOpenTitle );
        WREResOpenTitle = NULL;
    }
    if( WREResSaveTitle != NULL ) {
        FreeRCString( WREResSaveTitle );
        WREResSaveTitle = NULL;
    }
    if( WREResSaveIntoTitle != NULL ) {
        FreeRCString( WREResSaveIntoTitle );
        WREResSaveIntoTitle = NULL;
    }
    if( WREResSaveAsTitle != NULL ) {
        FreeRCString( WREResSaveAsTitle );
        WREResSaveAsTitle = NULL;
    }
    if( WREResFilter != NULL ) {
        FreeRCString( WREResFilter );
        WREResFilter = NULL;
    }
    if( WREResSaveMltFilter != NULL ) {
        FreeRCString( WREResSaveMltFilter );
        WREResSaveMltFilter = NULL;
    }
    if( WRESymSaveFilter != NULL ) {
        FreeRCString( WRESymSaveFilter );
        WRESymSaveFilter = NULL;
    }
    if( WRESymSaveTitle != NULL ) {
        FreeRCString( WRESymSaveTitle );
        WRESymSaveTitle = NULL;
    }
    if( WRESymLoadTitle != NULL ) {
        FreeRCString( WRESymLoadTitle );
        WRESymLoadTitle = NULL;
    }
    if( WREAccelFilter != NULL ) {
        FreeRCString( WREAccelFilter );
        WREAccelFilter = NULL;
    }
    if( WREMenuFilter != NULL ) {
        FreeRCString( WREMenuFilter );
        WREMenuFilter = NULL;
    }
    if( WREStringFilter != NULL ) {
        FreeRCString( WREStringFilter );
        WREStringFilter = NULL;
    }
}

static bool WREInitStaticVars( void )
{
    bool        ok;

    WREResUntitled = AllocRCString( WRE_UNTITLED );
    ok = (WREResUntitled != NULL);

    if( ok ) {
        WREResOpenTitle = AllocRCString( WRE_OPENPROJECTTITLE );
        ok = (WREResOpenTitle != NULL);
    }

    if( ok ) {
        WREResSaveTitle = AllocRCString( WRE_SAVEPROJECTTITLE );
        ok = (WREResSaveTitle != NULL);
    }

    if( ok ) {
        WREResSaveIntoTitle = AllocRCString( WRE_COPYINTOPROJECTTITLE );
        ok = (WREResSaveIntoTitle != NULL);
    }

    if( ok ) {
        WREResSaveAsTitle = AllocRCString( WRE_SAVEASPROJECTTITLE );
        ok = (WREResSaveAsTitle != NULL);
    }

    if( ok ) {
        WREResFilter = AllocRCString( WRE_PROJECTFILTER );
        ok = (WREResFilter != NULL);
        if( ok ) {
            WREMassageFilter( WREResFilter );
        }
    }

    if( ok ) {
        WREResSaveMltFilter = AllocRCString( WRE_SAVEMLTFILTER );
        ok = (WREResSaveMltFilter != NULL);
        if( ok ) {
            WREMassageFilter( WREResSaveMltFilter );
        }
    }

    if( ok ) {
        WRESymSaveFilter = AllocRCString( WRE_SYMFILTER );
        ok = (WRESymSaveFilter != NULL);
        if( ok ) {
            WREMassageFilter( WRESymSaveFilter );
        }
    }

    if( ok ) {
        WRESymLoadTitle = AllocRCString( WRE_LOADSYMTITLE );
        ok = (WRESymLoadTitle != NULL);
    }

    if( ok ) {
        WRESymSaveTitle = AllocRCString( WRE_SAVESYMTITLE );
        ok = (WRESymSaveTitle != NULL);
    }

    if( ok ) {
        WREAccelFilter = AllocRCString( WRE_ACCELFILTER );
        ok = (WREAccelFilter != NULL);
        if( ok ) {
            WREMassageFilter( WREAccelFilter );
        }
    }

    if( ok ) {
        WREMenuFilter = AllocRCString( WRE_MENUFILTER );
        ok = (WREMenuFilter != NULL);
        if( ok ) {
            WREMassageFilter( WREMenuFilter );
        }
    }

    if( ok ) {
        WREStringFilter = AllocRCString( WRE_STRINGFILTER );
        ok = (WREStringFilter != NULL);
        if( ok ) {
            WREMassageFilter( WREStringFilter );
        }
    }

    if( !ok ) {
        WREFiniStaticVars();
    }

    return( ok );
}

typedef struct infopadstruct {
    POINT       dlg_min_size;
    POINT       dlg_nc_size;
    POINT       dlg_border;
    int         text_y;
    POINT       type;
    POINT       total;
} infopadstruct;

static infopadstruct    InfoPad;
static bool             InfoPadInit = FALSE;

static void InitInfoPad( HWND info_dlg )
{
    HWND        win;
    RECT        rect1;

    GetWindowRect( info_dlg, &rect1 );
    InfoPad.dlg_min_size.x = rect1.right - rect1.left;
    InfoPad.dlg_min_size.y = rect1.bottom - rect1.top;
    GetClientRect( info_dlg, &rect1 );
    InfoPad.dlg_nc_size.x = InfoPad.dlg_min_size.x - rect1.right - rect1.left;
    InfoPad.dlg_nc_size.y = InfoPad.dlg_min_size.y - rect1.bottom - rect1.top;

    win = GetDlgItem( info_dlg, IDM_RNTYPETEXT );
    GetWindowRect( win, &rect1 );
    MapWindowPoints( (HWND)NULL, info_dlg, (POINT *)&rect1, 2 );
    InfoPad.dlg_border.x = rect1.left;
    InfoPad.dlg_border.y = rect1.top;
    InfoPad.type.x = rect1.right - rect1.left;
    InfoPad.type.y = rect1.bottom - rect1.top;

    win = GetDlgItem( info_dlg, IDM_RNTOTALTEXT );
    GetWindowRect( win, &rect1 );
    InfoPad.total.x = rect1.right - rect1.left;
    InfoPad.total.y = rect1.bottom - rect1.top;

    win = GetDlgItem( info_dlg, IDM_RNTYPE );
    GetWindowRect( win, &rect1 );
    MapWindowPoints( (HWND)NULL, info_dlg, (POINT *)&rect1, 2 );
    InfoPad.text_y = rect1.top - InfoPad.type.y - InfoPad.dlg_border.y;
}

static void WResizeInfoWindow( WREResInfo *info )
{
    HWND        win;
    RECT        rect;
    int         dwidth, dheight;
    int         x, y, width, height;
    bool        ok;

    if( info == NULL || info->info_win == (HWND)NULL ) {
        return;
    }

    if( !InfoPadInit ) {
        InitInfoPad( info->info_win );
        InfoPadInit = TRUE;
    }

    GetClientRect( info->res_win, &rect );
    dwidth = InfoPad.dlg_min_size.x;
    if( dwidth < rect.right - rect.left )
        dwidth = rect.right - rect.left;
    dheight = InfoPad.dlg_min_size.y;
    if( dheight < rect.bottom - rect.top )
        dheight = rect.bottom - rect.top;
    ok = SetWindowPos( info->info_win, (HWND)NULL, 0, 0, dwidth, dheight, SWP_NOZORDER ) != 0;

    if( ok ) {
        // resize the type list box
        win = GetDlgItem( info->info_win, IDM_RNTYPE );
        ok = (win != (HWND)NULL);
    }

    if( ok ) {
        width = dwidth - InfoPad.dlg_border.x * 3 - InfoPad.dlg_nc_size.x;
        height = dheight - InfoPad.dlg_border.y * 2 - InfoPad.type.y -
                 InfoPad.total.y - InfoPad.text_y * 2 - InfoPad.dlg_nc_size.y;
        ok = SetWindowPos( win, (HWND)NULL, 0, 0, (width * 2) / 5, height, SWP_NOMOVE | SWP_NOZORDER ) != 0;
    }

    if( ok ) {
        // move and size the resource listbox
        win = GetDlgItem( info->info_win, IDM_RNRES );
        ok = (win != (HWND)NULL);
    }

    if( ok ) {
        GetWindowRect( win, &rect );
        MapWindowPoints( (HWND)NULL, info->info_win, (POINT *)&rect, 2 );
        x = InfoPad.dlg_border.x * 2 + (width * 2) / 5;
        y = rect.top;
        ok = SetWindowPos( win, (HWND)NULL, x, y, (width * 3) / 5, height, SWP_NOZORDER ) != 0;
    }

    if( ok ) {
        // move the resource text static
        win = GetDlgItem( info->info_win, IDM_RNRESTEXT );
        ok = (win != (HWND)NULL);
    }

    if( ok ) {
        GetWindowRect( win, &rect );
        MapWindowPoints( (HWND)NULL, info->info_win, (POINT *)&rect, 2 );
        y = rect.top;
        ok = SetWindowPos( win, (HWND)NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER ) != 0;
    }

    if( ok ) {
        // move the resource total static
        win = GetDlgItem( info->info_win, IDM_RNTOTALTEXT );
        ok = (win != (HWND)NULL);
    }

    if( ok ) {
        GetWindowRect( win, &rect );
        MapWindowPoints( (HWND)NULL, info->info_win, (POINT *)&rect, 2 );
        x = x + (width * 3) / 5 - InfoPad.total.x;
        y = InfoPad.dlg_border.y + InfoPad.type.y + InfoPad.text_y * 2 + height;
        ok = SetWindowPos( win, (HWND)NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER ) != 0;
    }
}

bool PleaseOpenFile( UINT msg )
{
    char                *filter;
    char                *title;
    char                *name;
    WREResInfo          *res_info;
    WREResInfo          *old_info;
    WREGetFileStruct    gf;
    uint_16             type;
    bool                ok;

    old_info = WREGetCurrentRes();
    res_info = NULL;
    filter = NULL;
    title = NULL;
    name = NULL;

    if( msg == ACCEL_PLEASE_OPENME ) {
        filter = WREAccelFilter;
        title = AllocRCString( WRE_OPENACCEL );
        type = (uint_16)(pointer_int)RT_ACCELERATOR;
    } else if( msg == MENU_PLEASE_OPENME ) {
        filter = WREMenuFilter;
        title = AllocRCString( WRE_OPENMENU );
        type = (uint_16)(pointer_int)RT_MENU;
    } else if( msg == STRING_PLEASE_OPENME ) {
        filter = WREStringFilter;
        title = AllocRCString( WRE_OPENSTRING );
        type = (uint_16)(pointer_int)RT_STRING;
    }

    ok = (filter != NULL && title != NULL);

    if( ok ) {
        gf.file_name = NULL;
        gf.title = title;
        gf.filter = filter;
        gf.save_ext = FALSE;
        ok = ((name = WREGetOpenFileName( &gf )) != NULL);
    }

    if( ok ) {
        ok = ((res_info = WRELoadResource( name )) != NULL);
    }

    if( ok ) {
        WREFindAndLoadSymbols( res_info );
        ok = WRECreateResourceWindow( res_info );
    }

    if( ok ) {
        ListAddElt( &WREResList, (void *)res_info );
    }

    if( ok ) {
        ok = (WREFindTypeNode( res_info->info->dir, type, NULL ) != NULL);
    }

    if( ok ) {
        ok = WRESetResNamesFromType( res_info, type, FALSE, NULL, 0 );
    }

    if( ok ) {
        ok = WREHandleResEdit();
    }

    if( ok ) {
        SendMessage( old_info->res_win, WM_CLOSE, 0, 0 );
    }

    if( !ok ) {
        if( res_info != NULL ) {
            WREFreeResInfo( res_info );
            res_info = NULL;
        }
    }

    if( name != NULL ) {
        WRMemFree( name );
    }

    if( title != NULL ) {
        FreeRCString( title );
    }

    return( ok );
}

void WRESetCurrentRes( WREResInfo *res_info )
{
    WRECurrentRes = res_info;
}

WREResInfo *WREGetCurrentRes( void )
{
    return( WRECurrentRes );
}

bool WREIsCurrentMDIWindowZoomed( void )
{
    WREResInfo *info;

    info = WREGetCurrentRes();

    if( info != NULL && info->res_win != NULL ) {
        return( IsZoomed( info->res_win ) != 0 );
    } else {
        return( false );
    }
}

bool WRERegisterResClass( HINSTANCE app_inst )
{
    WNDCLASS wc;

    /* fill in the window class structure for the resource window */
    wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WREResWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = app_inst;
    wc.hIcon = LoadIcon( app_inst, "ResIcon" );
    wc.hCursor = NULL;
    //wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.hbrBackground = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WREResClass;

    /* register the resource window class */
    return( RegisterClass( &wc ) != 0 );
}

int WREIncNumRes( void )
{
    if( WRENumRes == 0 ) {
        WREEnableMenus( TRUE );
    }
    return( ++WRENumRes );
}

int WREDecNumRes( void )
{
    if( WRENumRes == 1 ) {
        WREEnableMenus( FALSE );
        if( WRENoInterface ) {
            PostMessage( WREGetMainWindowHandle(), WM_CLOSE, 0, 0 );
        }
    }
    return( WRENumRes != 0 ? --WRENumRes : WRENumRes );
}

int WREGetNumRes( void )
{
    return( WRENumRes );
}

WREResInfo *WRECreateNewResource( char *filename )
{
    WREResInfo  *res_info;
    bool        ok;

    ok = ((res_info = WREAllocResInfo()) != NULL);

    if( ok ) {
        res_info->info = WRAllocWRInfo();
        ok = (res_info->info != NULL);
    }

    if( ok ) {
        res_info->symbol_table = WRInitHashTable();
        ok = (res_info->symbol_table != NULL);
    }

    if( ok && filename != NULL ) {
        res_info->info->save_name = WREStrDup( filename );
        ok = (res_info->info->save_name != NULL);
    }

    if( ok ) {
#ifdef __NT__
        res_info->is32bit = TRUE;
#else
        res_info->is32bit = FALSE;
#endif
        if( res_info->is32bit ) {
            res_info->info->internal_type = WR_WINNTW_RES;
        } else {
            res_info->info->internal_type = WR_WIN16W_RES;
        }
        WREFindAndLoadSymbols( res_info );
        ok = WRECreateResourceWindow( res_info );
    }

    if( res_info != NULL ) {
        if( ok ) {
            ListAddElt( &WREResList, (void *)res_info );
        } else {
            WREFreeResInfo( res_info );
            res_info = NULL;
        }
    }

    return( res_info );
}

bool WREOpenResource( char *fn )
{
    char                *name;
    WREResInfo          *res_info;
    WREGetFileStruct    gf;
    bool                ok, got_name;

    res_info = NULL;
    name = NULL;
    got_name = FALSE;

    if( fn != NULL ) {
        if( WRFileExists( fn ) ) {
            ok = ((name = WREStrDup( fn )) != NULL);
        } else {
            ok = false;
        }
    } else {
        gf.file_name = NULL;
        gf.title = WREResOpenTitle;
        gf.filter = WREResFilter;
        gf.save_ext = TRUE;
        ok = ((name = WREGetOpenFileName( &gf )) != NULL);
    }

    if( ok ) {
        got_name = TRUE;
        ok = ((res_info = WRELoadResource( name )) != NULL);
    }

    if( ok ) {
        WREFindAndLoadSymbols( res_info );
        ok = WRECreateResourceWindow( res_info );
    }

    if( ok ) {
        ListAddElt( &WREResList, (void *)res_info );
    } else {
        if( res_info != NULL ) {
            WREFreeResInfo( res_info );
            res_info = NULL;
        }
        if( got_name ) {
            WREDisplayErrorMsg( WRE_RESOURCESNOTLOADED );
        }
    }

    if( name != NULL ) {
        WRMemFree( name );
    }

    return( ok );
}

WREResInfo *WRELoadResource( const char *file_name )
{
    WRFileType  file_type;
    WREResInfo  *res_info;
    bool        ok;

    WRESetWaitCursor( TRUE );

    ok = ((res_info = WREAllocResInfo()) != NULL);

    if( ok ) {
        file_type = WRIdentifyFile( file_name );
        ok = (file_type != WR_INVALID_FILE);
    }

    if( ok ) {
        res_info->info = WRLoadResource( file_name, file_type );
        ok = (res_info->info != NULL);
    }

    if( ok ) {
        file_type = res_info->info->file_type;
        if( res_info->info->internal_type != WR_DONT_KNOW ) {
            file_type = res_info->info->internal_type;
        }
        res_info->is32bit = WRIs32Bit( file_type );
    }

    if( ok ) {
        res_info->symbol_table = WRInitHashTable();
        ok = (res_info->symbol_table != NULL);
    }

    if( !ok ) {
        if( res_info != NULL ) {
            WREFreeResInfo( res_info );
            res_info = NULL;
        }
    }

    WRESetWaitCursor( FALSE );

    return( res_info );
}

WREResInfo *WREResInfoFromWin( HWND win )
{
    WREResInfo *info;
    LIST       *rlist;

    if( win != NULL ) {
        info = NULL;
        for( rlist = WREResList; rlist != NULL; rlist = ListNext( rlist ) ) {
            info = (WREResInfo *)ListElement( rlist );
            if( info->res_win == win ) {
                return( info );
            }
        }
    }

    return( NULL );
}

bool WREIsResInfoWinMsg( LPMSG pmsg )
{
    WREResInfo *info;
    LIST       *rlist;

    info = NULL;
    for( rlist = WREResList; rlist != NULL; rlist = ListNext( rlist ) ) {
        info = (WREResInfo *)ListElement( rlist );
        if( info->info_win != (HWND)NULL ) {
            if( IsDialogMessage( info->info_win, pmsg ) ) {
                return( TRUE );
            }
        }
    }

    return( FALSE );
}

void WREActivateResourceWindow( WREResInfo *res_info, WPARAM wParam, LPARAM lParam )
{
    WREResInfo  *info;
    bool        fActivate;
    HWND        hwndDeact;

    _wre_touch( wParam );

    if( res_info != NULL ) {
        fActivate = GET_WM_MDIACTIVATE_FACTIVATE( res_info->res_win, wParam, lParam ) != 0;
        hwndDeact = GET_WM_MDIACTIVATE_HWNDDEACTIVATE( wParam, lParam );
        res_info->active = fActivate;
        if( fActivate ) {
            WRESetCurrentRes( res_info );
            if( res_info->info_win != (HWND)NULL ) {
                SetFocus( res_info->info_win );
            }
            info = WREResInfoFromWin( hwndDeact );
            if( info != NULL ) {
                info->active = FALSE;
            }
        }
    }
}

char *WREGetQueryName( WREResInfo *res_info )
{
    char *name;

    if( res_info->info->save_name != NULL ) {
        name = res_info->info->save_name;
    } else if( res_info->info->file_name != NULL ) {
        name = res_info->info->file_name;
    } else {
        name = WREResUntitled;
    }

    return( name );
}

static void WRECheckIfActiveWindow( void )
{
    HWND        main;
    HWND        active;

    main = WREGetMainWindowHandle();
    active = GetActiveWindow();
    if( active != main ) {
        //SetActiveWindow( main );
        ShowWindow( main, SW_RESTORE );
        BringWindowToTop( main );
    }
}

bool WREQuerySaveSymOnDeleteRes( WREResInfo *res_info, bool fatal_exit )
{
    int         ret;
    UINT        style;
    char        *text;
    char        *file;
    HWND        frame;

    if( WRENoInterface ) {
        return( TRUE );
    }

    if( res_info == NULL || res_info->symbol_table == NULL ) {
        return( TRUE );
    }

    if( WRIsHashTableDirty( res_info->symbol_table ) ) {
        if( fatal_exit ) {
            style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
        } else {
            style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
        }
        WRECheckIfActiveWindow();
        frame = WREGetMDIWindowHandle();
        SendMessage( frame, WM_MDIRESTORE, (WPARAM)res_info->res_win, 0 );
        SendMessage( frame, WM_MDIACTIVATE, (WPARAM)res_info->res_win, 0 );
        file = WREGetQueryName( res_info );
        text = AllocRCString( WRE_SAVEMODIFIEDSYM );
        ret = MessageBox( res_info->res_win, text, file, style );
        if( text != NULL ) {
            FreeRCString( text );
        }
        if( ret == IDYES ) {
            if( res_info->symbol_file == NULL ) {
                res_info->symbol_file = WRECreateSymName( file );
            }
            if( !WRESaveSymbols( res_info->symbol_table, &res_info->symbol_file, FALSE ) ) {
                return( FALSE );
            }
        } else if( ret == IDCANCEL ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

bool WREQuerySaveResOnDeleteRes( WREResInfo *res_info, bool fatal_exit )
{
    int         ret;
    UINT        style;
    HWND        frame;
    char        *text;

    if( WRENoInterface ) {
        return( TRUE );
    }

    if( res_info != NULL && WREIsResModified( res_info ) ) {
        if( fatal_exit ) {
            style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
        } else {
            style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
        }
        WRECheckIfActiveWindow();
        frame = WREGetMDIWindowHandle();
        SendMessage( frame, WM_MDIRESTORE, (WPARAM)res_info->res_win, 0 );
        SendMessage( frame, WM_MDIACTIVATE, (WPARAM)res_info->res_win, 0 );
        text = AllocRCString( WRE_QUERYMODIFIED );
        ret = MessageBox( res_info->res_win, text, WREGetQueryName( res_info ), style );
        if( text != NULL ) {
            FreeRCString( text );
        }
        if( ret == IDYES ) {
            return( WRESaveResource( res_info, FALSE ) );
        } else if( ret == IDCANCEL ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

bool WRESaveResource( WREResInfo *res_info, bool get_name )
{
    char                *fn;
    WREGetFileStruct    gf;
    int                 fn_offset;
    bool                got_name;
    bool                ok;

    fn_offset = 0;
    got_name = FALSE;

    ok = (res_info != NULL && res_info->info != NULL);

    if( ok ) {
        ok = (WRCountZeroLengthResources( res_info->info->dir ) == 0);
        if( !ok ) {
            WREDisplayErrorMsg( WRE_UPDATEBEFORESAVE );
        }
    }

    if( ok ) {
        if( res_info->info->save_name != NULL ) {
            fn = res_info->info->save_name;
        } else {
            res_info->info->save_type = res_info->info->file_type;
            fn = WREStrDup( res_info->info->file_name );
            got_name = TRUE;
        }

        if( get_name || fn == NULL || *fn == '\0' ) {
            gf.file_name = fn;
            gf.title = WREResSaveTitle;
            gf.filter = WREResFilter;
            gf.save_ext = TRUE;
            fn = WREGetSaveFileName( &gf );
            got_name = TRUE;
            res_info->info->save_type = WR_DONT_KNOW;
        }

        ok = (fn != NULL && *fn != '\0');
    }

    if( ok ) {
        if( got_name && res_info->info->save_name != NULL ) {
            WRMemFree( res_info->info->save_name );
        }
        res_info->info->save_name = fn;
        if( res_info->info->save_type == WR_DONT_KNOW ) {
            res_info->info->save_type = WRESelectFileType( fn, res_info->is32bit );
        }
        ok = (res_info->info->save_type != WR_DONT_KNOW);
    }

    if( ok ) {
        if( WRIsHashTableDirty( res_info->symbol_table ) ) {
            if( res_info->symbol_file == NULL ) {
                res_info->symbol_file = WRECreateSymName( fn );
            }
        }
    }

    if( ok ) {
        WRECreateDLGInclude( &res_info->info->dir, res_info->symbol_file );
        ok = WRESaveResourceToFile( res_info );
        if( !ok ) {
            WREDisplayErrorMsg( WRE_SAVEFAILED );
        }
    }

    if( ok ) {
        if( get_name || WRIsHashTableDirty( res_info->symbol_table ) ) {
            ok = WRESaveSymbols( res_info->symbol_table, &res_info->symbol_file, get_name );
        }
    }

    if( ok ) {
        //fn_offset = WRFindFnOffset( fn );
        SendMessage( res_info->res_win, WM_SETTEXT, 0, (LPARAM)(LPSTR)&fn[fn_offset] );
    }

    return( ok );
}

bool WRESaveResourceToFile( WREResInfo *res_info )
{
    bool          ok;

    WRESetWaitCursor( TRUE );

    ok = (res_info != NULL && res_info->info != NULL);

    if( ok ) {
        ok = WRSaveResource( res_info->info, true );
    }

    if( ok ) {
        WRESetResModified( res_info, FALSE );
    }

    WRESetWaitCursor( FALSE );

    return( ok );
}

bool WREQueryKillApp( bool fatal_exit )
{
    LIST        *rlist;
    WREResInfo  *info;
    bool        kill_app;

    if( !WRENoInterface && WREResList != NULL ) {
        kill_app = TRUE;
        for( rlist = WREResList; rlist != NULL && kill_app; rlist = ListNext( rlist ) ) {
            info = (WREResInfo *)ListElement( rlist );
            kill_app = WREQuerySaveResOnDeleteRes( info, fatal_exit ) &&
                       WREQuerySaveSymOnDeleteRes( info, fatal_exit );
            kill_app = kill_app || fatal_exit;
        }
        if( !fatal_exit && !kill_app ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

void WREFreeResList( void )
{
    LIST        *rlist;
    WREResInfo  *info;
    HWND        frame;

    if( WREResList != NULL ) {
        frame = WREGetMDIWindowHandle();
        for( rlist = WREResList; rlist != NULL; rlist = ListNext( rlist ) ) {
            info = (WREResInfo *)ListElement( rlist );
            SendMessage( frame, WM_MDIACTIVATE, (WPARAM)info->res_win, 0 );
            WREFreeResInfo( info );
        }
        ListFree( WREResList );
        WREResList = NULL;
        WRENumRes = 0;
    }
}

bool WRERemoveResource( WREResInfo *res_info )
{
    LIST *node;

    if( WREResList == NULL ) {
        return( FALSE );
    }

    if( (node = ListFindElt( WREResList, res_info )) != NULL ) {
        ListRemoveElt( &WREResList, (void *)ListElement( node ) );
    } else {
        return( FALSE );
    }

    WREFreeResInfo( res_info );

    WREDecNumRes();

    if( !WREGetNumRes() ) {
        WRESetCurrentRes( NULL );
    }

    return( TRUE );
}

bool WRECreateResourceWindow( WREResInfo *res_info )
{
    MDICREATESTRUCT     mdics;
    LRESULT             ret;
    HWND                win;
    bool                ok;
    DWORD               style;
    char                *win_title;
    int                 win_title_len;

    win_title = NULL;

    ok = (res_info != NULL);

    if( ok ) {
        WREIncNumRes();
        if( res_info->info->file_name != NULL ) {
            //int     fn_offset;
            //perhaps I should make this an option
            //fn_offset = WRFindFnOffset( res_info->info->file_name );
            //title = &res_info->info->file_name[fn_offset];
            mdics.szTitle = res_info->info->file_name;
        } else if( res_info->info->save_name != NULL ) {
            mdics.szTitle = res_info->info->save_name;
        } else {
            WREResCounter++;
            win_title_len = strlen( WREResUntitled ) + 7;
            win_title = (char *)WRMemAlloc( win_title_len );
            sprintf( win_title, "%s.%d", WREResUntitled, 0xffff & WREResCounter );
            mdics.szTitle = win_title;
        }
        style = 0;
        if( WREGetNumRes() != 1 && WREIsCurrentMDIWindowZoomed() ) {
            style = WS_MAXIMIZE;
        }
        mdics.szClass = WREResClass;
        mdics.hOwner = WREAppInst;
        mdics.x = CW_USEDEFAULT;
        mdics.y = CW_USEDEFAULT;
        mdics.cx = CW_USEDEFAULT;
        mdics.cy = CW_USEDEFAULT;
        mdics.style = style;
        mdics.lParam = (LPARAM)(LPVOID)res_info;

        ret = SendMessage( WREGetMDIWindowHandle(), WM_MDICREATE, 0, (LPARAM)(LPVOID)&mdics );
        ok = (ret != 0);
    }

    if( ok ) {
#ifdef __NT__
        win = (HWND)ret;
#else
        win = (HWND)LOWORD( ret );
#endif
        ok = (res_info->res_win != NULL && res_info->res_win == win);
    }

    if( ok ) {
        ok = WRECreateResInfoWindow( res_info );
    }

    if( ok ) {
        WResizeInfoWindow( res_info );
        ShowWindow( res_info->info_win, SW_SHOW );
        SetFocus( res_info->info_win );
    }

    if( win_title != NULL ) {
        WRMemFree( win_title );
    }

    return( ok );
}

bool WREDestroyResourceWindow( WREResInfo *res_info )
{
    if( res_info != NULL && WREQuerySaveResOnDeleteRes( res_info, FALSE ) &&
        WREQuerySaveSymOnDeleteRes( res_info, FALSE ) ) {
        return( WRERemoveResource( res_info ) );
    }
    return( FALSE );
}

LRESULT CALLBACK WREResWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREResInfo  *res_info;
    int         msg_processed;
    LRESULT     ret;

    msg_processed = FALSE;
    ret = FALSE;
    res_info = NULL;

    switch( message ) {
    case WM_CREATE:
        res_info = (WREResInfo *)((MDICREATESTRUCT *)((CREATESTRUCT *)lParam)->lpCreateParams)->lParam;
        res_info->res_win = hWnd;
        SET_WNDINFO( hWnd, (LONG_PTR)res_info );
        break;
    case WM_SIZE:
    case WM_MDIACTIVATE:
    case WM_CLOSE:
    case WM_SETFOCUS:
        res_info = (WREResInfo *)GET_WNDINFO( hWnd );
        break;
    case WM_DESTROY:
        SET_WNDINFO( hWnd, (LONG_PTR)NULL );
        break;
    }

    if( res_info != NULL ) {
        switch( message ) {
        case WM_SETFOCUS:
            if( res_info->info_win != (HWND)NULL ) {
                SetFocus( res_info->info_win );
            }
            break;
        case WM_SIZE:
            WResizeInfoWindow( res_info );
            break;

        case WM_MDIACTIVATE:
            WREActivateResourceWindow( res_info, wParam, lParam );
            break;

        case WM_CLOSE:
            WREEndResAccelSessions( res_info );
            WREEndResMenuSessions( res_info );
            WREEndResStringSessions( res_info );
            WREEndResDialogSessions( res_info );
            WREEndResImageSessions( res_info );
            WREDestroyResourceWindow( res_info );
            return( (LRESULT)FALSE );
        }
    }

    if( !msg_processed ) {
        ret = DefMDIChildProc( hWnd, message, wParam, lParam );
    }

    return( ret );
}

bool WREInitResources( HINSTANCE inst )
{
    WREResInfoBrush = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
    WREAppInst = inst;
    WREResInfoWinProc = (DLGPROC)MakeProcInstance( (FARPROC)WREResInfoProc, inst );
    return( WREInitStaticVars() );
}

void WREFiniResources( void )
{
    if( WREResInfoBrush != NULL ) {
        DeleteObject( WREResInfoBrush );
    }
    FreeProcInstance( (FARPROC)WREResInfoWinProc );
    WREFiniStaticVars();
}

bool WRECreateResInfoWindow( WREResInfo *info )
{
    info->info_win = JCreateDialogParam( WREAppInst, "WREResource", info->res_win, WREResInfoWinProc, (LPARAM)(LPVOID)info );

    if( info->info_win == (HWND)NULL ) {
        return( FALSE );
    }

    return( TRUE );
}

LRESULT CALLBACK WREResInfoProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREResInfo  *info;
    LRESULT     ret;
    WORD        wp;
    UINT        cmd;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        info = (WREResInfo *)lParam;
        info->info_win = hDlg;
        SET_DLGDATA( hDlg, info );
        if( !WREInitResourceWindow( info, 0 ) ) {
            DestroyWindow( hDlg );
        } else {
            ret = TRUE;
        }
        break;

    case WM_SYSCOLORCHANGE:
#if defined( __NT__ )
        SET_HBRBACKGROUND( hDlg, (HBRUSH)(COLOR_BTNFACE + 1) );
#endif
        WRECtl3dColorChange();
        break;

    case WM_COMMAND:
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDM_RNRES:
            cmd = GET_WM_COMMAND_CMD( wParam, lParam );
            if( cmd == LBN_DBLCLK ) {
                WREHandleResEdit();
            }
            break;

        case IDM_RNTYPE:
            info = (WREResInfo *)GET_DLGDATA( hDlg );
            cmd = GET_WM_COMMAND_CMD( wParam, lParam );
            if( cmd == LBN_SELCHANGE ) {
                WREAddResNames( info );
            }
            break;
        }
        break;
    }

    return( ret );
}
