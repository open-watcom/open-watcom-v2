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
* Description:  Heap walker configuration.
*
****************************************************************************/


#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include "heapwalk.h"
#include "watini.h"
#include "jdlg.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL ConfigDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );

#define         SECT_NAME       "WATCOM heapwalker"
#define         LSORT           "LSortType"
#define         GSORT           "GSortType"
#define         GSAVE           "GSaveName"
#define         LSAVE           "LSaveName"
#define         DISP_TYPE       "display_type"
#define         DISP_RES        "display_resources"
#define         GLOB_XPOS       "glob_wnd_x_pos"
#define         GLOB_YPOS       "glob_wnd_y_pos"
#define         GLOB_XSIZE      "glob_wnd_x_size"
#define         GLOB_YSIZE      "glob_wnd_y_size"
#define         SAVE_GLOB_POS   "save_glob_wnd_pos"
#define         SAVE_MW_POS     "save_mem_wnd_pos"
#define         MW_XPOS         "memwnd_xpos"
#define         MW_YPOS         "memwnd_ypos"
#define         MW_XSIZE        "memwnd_xsize"
#define         MW_YSIZE        "memwnd_ysize"
#define         MW_FNAME        "memwnd_fname"
#define         MW_MAX          "memwnd_max"
#define         MW_DISP_INFO    "memwnd_disp_info"
#define         MW_AUTOPOS      "memwnd_autopos"
#define         MW_NO_UPDT      "memwnd_no_update"
#define         MW_1_WND        "memwnd_single_wnd"
#define         MW_DISP_TYPE    "memwnd_disp_type"
#define         MW_CODE_TYPE    "memwnd_code_type"


/*
 * ValidateFName - ensure that the path specified by 'path' actually exists
 */
static BOOL ValidateFName( char *path ) {

    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        name[_MAX_PATH];
    DIR         *info;

    _splitpath( path, drive, dir, NULL, NULL );
    _makepath( name, drive, dir, "*", "*" );
    info = opendir( name );
    if( info == NULL ) {
        return( FALSE );
    } else {
        closedir( info );
        return( TRUE );
    }
} /* ValidateFName */

/*
 * PutProfileBool - write a boolean value to the configuration file
 */
static void PutProfileBool( char *id, BOOL val ) {

    char        buf[15];

    if( val ) {
        itoa( TRUE, buf, 10 );
    } else {
        itoa( FALSE, buf, 10 );
    }
    WritePrivateProfileString( SECT_NAME, id, buf, WATCOM_INI );
} /* PutProfileBool */

/*
 * GetDefaults - set 'info' to the default configuration values
 */
static void GetDefaults( HeapConfigInfo *info ) {

    WORD        x;
    WORD        y;
    char        *str;

    info->save_glob_pos = TRUE;
    info->save_mem_pos = TRUE;
    info->disp_res = TRUE;
    x = GetSystemMetrics( SM_CXSCREEN );
    y = GetSystemMetrics( SM_CYSCREEN );
    info->glob_xpos = x / 32;
    info->glob_ypos = y / 8;
    info->glob_xsize = 15 * ( x / 16 );
    info->glob_ysize = 3 * ( y / 4 );
    str = HWGetRCString( STR_DEF_GLOB_HEAP_FNAME );
    strcpy( info->gfname, str );
    str = HWGetRCString( STR_DEF_LCL_HEAP_FNAME );
    strcpy( info->lfname, str );
} /* GetDefaults */

/*
 * ReadConfig - read configuration information from the .ini file
 */
void ReadConfig( void ) {

    MemWndConfig        info;

    GetDefaults( &Config );
    LSortType = GetPrivateProfileInt( SECT_NAME, LSORT,
                                HEAPMENU_SORT_HANDLE, WATCOM_INI );
    GSortType = GetPrivateProfileInt( SECT_NAME, GSORT,
                                HEAPMENU_SORT_HANDLE, WATCOM_INI );
    HeapType = GetPrivateProfileInt( SECT_NAME, DISP_TYPE,
                                HEAPMENU_DISPLAY_ENTIRE, WATCOM_INI );
    Config.disp_res = GetPrivateProfileInt( SECT_NAME, DISP_RES,
                                        Config.disp_res, WATCOM_INI );
    Config.save_glob_pos = GetPrivateProfileInt( SECT_NAME, SAVE_GLOB_POS,
                                        Config.save_glob_pos, WATCOM_INI );
    Config.save_mem_pos = GetPrivateProfileInt( SECT_NAME, SAVE_MW_POS,
                                        Config.save_mem_pos, WATCOM_INI );
    Config.glob_xpos = GetPrivateProfileInt( SECT_NAME, GLOB_XPOS,
                                        Config.glob_xpos, WATCOM_INI );
    Config.glob_ypos = GetPrivateProfileInt( SECT_NAME, GLOB_YPOS,
                                        Config.glob_ypos, WATCOM_INI );
    Config.glob_xsize = GetPrivateProfileInt( SECT_NAME, GLOB_XSIZE,
                                        Config.glob_xsize, WATCOM_INI );
    Config.glob_ysize = GetPrivateProfileInt( SECT_NAME, GLOB_YSIZE,
                                        Config.glob_ysize, WATCOM_INI );
    GetPrivateProfileString( SECT_NAME, GSAVE, Config.gfname,
                        Config.gfname, _MAX_PATH, WATCOM_INI );
    GetPrivateProfileString( SECT_NAME, LSAVE, Config.lfname,
                        Config.lfname, _MAX_PATH, WATCOM_INI );

    /* read information about the memory window */

    GetMemWndConfig( &info );
    info.xpos = GetPrivateProfileInt( SECT_NAME, MW_XPOS,
                                        info.xpos, WATCOM_INI );
    info.ypos = GetPrivateProfileInt( SECT_NAME, MW_YPOS,
                                        info.ypos, WATCOM_INI );
    info.xsize = GetPrivateProfileInt( SECT_NAME, MW_XSIZE,
                                        info.xsize, WATCOM_INI );
    info.ysize = GetPrivateProfileInt( SECT_NAME, MW_YSIZE,
                                        info.ysize, WATCOM_INI );
    GetPrivateProfileString( SECT_NAME, MW_FNAME, info.fname, info.fname,
                                        _MAX_PATH, WATCOM_INI );
    info.maximized = GetPrivateProfileInt( SECT_NAME, MW_MAX,
                                        info.maximized, WATCOM_INI );
    info.disp_info = GetPrivateProfileInt( SECT_NAME, MW_DISP_INFO,
                                        info.disp_info, WATCOM_INI );
    info.autopos_info = GetPrivateProfileInt( SECT_NAME, MW_AUTOPOS,
                                        info.autopos_info, WATCOM_INI );
    info.forget_pos = GetPrivateProfileInt( SECT_NAME, MW_NO_UPDT,
                                        info.forget_pos, WATCOM_INI );
    info.allowmult = GetPrivateProfileInt( SECT_NAME, MW_1_WND,
                                        info.allowmult, WATCOM_INI );
    info.data_type = GetPrivateProfileInt( SECT_NAME, MW_DISP_TYPE,
                                        info.data_type, WATCOM_INI );
    info.code_type = GetPrivateProfileInt( SECT_NAME, MW_CODE_TYPE,
                                        info.code_type, WATCOM_INI );
    SetMemWndConfig( &info );
    InitMonoFont( SECT_NAME, WATCOM_INI, SYSTEM_FIXED_FONT, Instance );
} /* ReadConfig */


/*
 * SaveConfigFile - save configuration information to the .ini file
 */
void SaveConfigFile( BOOL save_all_values ) {

    char                buf[25];
    MemWndConfig        info;

    utoa( LSortType, buf, 10 );
    WritePrivateProfileString( SECT_NAME, LSORT, buf, WATCOM_INI );
    utoa( GSortType, buf, 10 );
    WritePrivateProfileString( SECT_NAME, GSORT, buf, WATCOM_INI );
    utoa( HeapType, buf, 10 );
    WritePrivateProfileString( SECT_NAME, DISP_TYPE, buf, WATCOM_INI );
    if( Config.save_glob_pos || save_all_values ) {
        itoa( Config.glob_xpos, buf, 10 );
        WritePrivateProfileString( SECT_NAME, GLOB_XPOS, buf, WATCOM_INI );
        itoa( Config.glob_ypos, buf, 10 );
        WritePrivateProfileString( SECT_NAME, GLOB_YPOS, buf, WATCOM_INI );
        utoa( Config.glob_xsize, buf, 10 );
        WritePrivateProfileString( SECT_NAME, GLOB_XSIZE, buf, WATCOM_INI );
        utoa( Config.glob_ysize, buf, 10 );
        WritePrivateProfileString( SECT_NAME, GLOB_YSIZE, buf, WATCOM_INI );
    }
    PutProfileBool( DISP_RES, Config.disp_res );
    PutProfileBool( SAVE_GLOB_POS, Config.save_glob_pos );
    PutProfileBool( SAVE_MW_POS, Config.save_mem_pos );
    WritePrivateProfileString( SECT_NAME, GSAVE, Config.gfname, WATCOM_INI );
    WritePrivateProfileString( SECT_NAME, LSAVE, Config.lfname, WATCOM_INI );

    /* save information about the memory window */
    GetMemWndConfig( &info );
    if( Config.save_mem_pos || save_all_values ) {
        itoa( info.xpos, buf, 10 );
        WritePrivateProfileString( SECT_NAME, MW_XPOS, buf, WATCOM_INI );
        itoa( info.ypos, buf, 10 );
        WritePrivateProfileString( SECT_NAME, MW_YPOS, buf, WATCOM_INI );
        utoa( info.xsize, buf, 10 );
        WritePrivateProfileString( SECT_NAME, MW_XSIZE, buf, WATCOM_INI );
        utoa( info.ysize, buf, 10 );
        WritePrivateProfileString( SECT_NAME, MW_YSIZE, buf, WATCOM_INI );
    }
    WritePrivateProfileString( SECT_NAME, MW_FNAME, info.fname,
                               WATCOM_INI );
    PutProfileBool( MW_MAX, info.maximized );
    PutProfileBool( MW_DISP_INFO, info.disp_info );
    PutProfileBool( MW_AUTOPOS, info.autopos_info );
    PutProfileBool( MW_NO_UPDT, info.forget_pos );
    utoa( info.allowmult, buf, 10 );
    WritePrivateProfileString( SECT_NAME, MW_1_WND, buf, WATCOM_INI );
    utoa( info.data_type, buf, 10 );
    WritePrivateProfileString( SECT_NAME, MW_DISP_TYPE, buf, WATCOM_INI );
    utoa( info.code_type, buf, 10 );
    WritePrivateProfileString( SECT_NAME, MW_CODE_TYPE, buf, WATCOM_INI );
    SaveMonoFont( SECT_NAME, WATCOM_INI );
}/* SaveConfigFile */

/*
 * SetupConfigDlg - set the fields in the configuration dialog to
 *                  reflect the values in 'heap' and 'mem'
 */
static void SetupConfigDlg( HeapConfigInfo *heap, MemWndConfig *mem, HWND hwnd ) {

    SetDlgItemText( hwnd, CONFIG_GNAME, heap->gfname  );
    SetDlgItemText( hwnd, CONFIG_LNAME, heap->lfname  );
    SetDlgItemText( hwnd, CONFIG_MNAME, mem->fname );
    if( mem->autopos_info ) {
        CheckDlgButton( hwnd, CONFIG_AUTOPOS, TRUE );
    }
    if( mem->disp_info ) {
        CheckDlgButton( hwnd, CONFIG_DISP_MEM_INFO, TRUE );
    }
    if( mem->allowmult == WND_MULTI ) {
        CheckDlgButton( hwnd, CONFIG_MULT_MEM_WND, TRUE );
    }
    if( heap->disp_res ) {
        CheckDlgButton( hwnd, CONFIG_DISP_RES, TRUE );
    }
    if( heap->save_glob_pos ) {
        CheckDlgButton( hwnd, CONFIG_SAVE_MAIN_POS, TRUE );
    }
    if( heap->save_mem_pos ) {
        CheckDlgButton( hwnd, CONFIG_SAVE_MEM_POS, TRUE );
    }
} /* SetupConfigDlg */


static void doFileBrowse( HWND hwnd, WORD id ) {

    char        buf[_MAX_PATH];

    if( GetSaveFName( hwnd, buf ) ) {
        SetDlgItemText( hwnd, id, buf );
    }
}


/*
 * ConfigDlgProc - handle messages from the configure dialog
 */
BOOL FAR PASCAL ConfigDlgProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam )
{
    MemWndConfig        info;
    HeapConfigInfo      heapdef;
    char                buf[_MAX_PATH];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        GetMemWndConfig( &info );
        SetupConfigDlg( &Config, &info, hwnd );
        break;
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case CONFIG_GLOB_BROWSE:
            doFileBrowse( hwnd, CONFIG_GNAME );
            break;
        case CONFIG_LOCAL_BROWSE:
            doFileBrowse( hwnd, CONFIG_LNAME );
            break;
        case CONFIG_MEM_BROWSE:
            doFileBrowse( hwnd, CONFIG_MNAME );
            break;
        case CONFIG_DEFAULT:
            GetMemWndDefault( &info );
            GetDefaults( &heapdef );
            SetupConfigDlg( &heapdef, &info, hwnd );
            break;
        case CONFIG_OK:
            GetDlgItemText( hwnd, CONFIG_GNAME, buf, _MAX_PATH );
            if( !ValidateFName( buf ) ) {
                GetWindowText( hwnd, buf, sizeof( buf ) );
                RCMessageBox( hwnd, STR_INVALID_GLOB_FNAME,
                              buf, MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            GetDlgItemText( hwnd, CONFIG_LNAME, buf, _MAX_PATH );
            if( !ValidateFName( buf ) ) {
                GetWindowText( hwnd, buf, sizeof( buf ) );
                RCMessageBox( hwnd, STR_INVALID_LCL_FNAME, buf,
                            MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            GetDlgItemText( hwnd, CONFIG_MNAME, buf, _MAX_PATH );
            if( !ValidateFName( buf ) ) {
                GetWindowText( hwnd, buf, sizeof( buf ) );
                RCMessageBox( hwnd, STR_INVALID_MEM_FNAME, buf,
                            MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            GetMemWndConfig( &info );
            info.autopos_info = IsDlgButtonChecked( hwnd, CONFIG_AUTOPOS );
            info.disp_info = IsDlgButtonChecked( hwnd, CONFIG_DISP_MEM_INFO );
            if( IsDlgButtonChecked( hwnd, CONFIG_MULT_MEM_WND ) ) {
                info.allowmult = WND_MULTI;
            } else {
                info.allowmult = WND_REPLACE;
            }
            Config.disp_res = IsDlgButtonChecked( hwnd, CONFIG_DISP_RES );
            Config.save_glob_pos = IsDlgButtonChecked( hwnd,
                                                       CONFIG_SAVE_MAIN_POS );
            Config.save_mem_pos = IsDlgButtonChecked( hwnd,
                                                       CONFIG_SAVE_MEM_POS );
            GetDlgItemText( hwnd, CONFIG_GNAME, Config.gfname, _MAX_PATH );
            GetDlgItemText( hwnd, CONFIG_LNAME, Config.lfname, _MAX_PATH );
            GetDlgItemText( hwnd, CONFIG_MNAME, info.fname, _MAX_PATH );
            SetMemWndConfig( &info );
            /* fall through */
        case CONFIG_CANCEL:
            EndDialog( hwnd, FALSE );
            break;
        default:
            return( FALSE );
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
} /* ConfigDlgProc */

/*
 * HWConfigure - get configuration information from the user
 */
void HWConfigure( void ) {

    FARPROC             fp;

    fp = MakeProcInstance( (FARPROC)ConfigDlgProc, Instance );
    JDialogBox( Instance, "HEAP_CONFIG", HeapWalkMainWindow, (DLGPROC)fp );
    FreeProcInstance( fp );
} /* HWConfigure */
