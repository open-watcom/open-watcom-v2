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


#include "vi.h"
#include "setgen.h"
#include "stddef.h"
#include "ctltype.h"
#include "dyntpl.h"
#include "util.h"
#include "globals.h"
#include "wprocmap.h"
#include "winctl.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT BOOL CALLBACK SetGenProc( HWND hwndDlg, UINT msg, WPARAM wparam, LPARAM lparam );

#define WORDWIDTH               30
#define TMPDIRWIDTH             129
#define HISTORYFILEWIDTH        129

typedef struct {
    int         AutoSaveInterval;
    // int         MaxLineLen;
    bool        Undo :1;
    bool        AutoSave :1;
    bool        LastEOL :1;
    bool        SaveConfig :1;
    bool        SaveOnBuild :1;
    bool        BeepFlag :1;
    bool        QuitMovesForward :1;
    bool        SameFileCheck :1;
    bool        Modal :1;
    bool        CaseIgnore :1;
    bool        SearchWrap :1;
    char        Word[WORDWIDTH];
    char        WordAlt[WORDWIDTH];
    char        TmpDir[TMPDIRWIDTH];
    char        HistoryFile[HISTORYFILEWIDTH];
} dlg_data;

extern  char        *WordDefnDefault;

static  dlg_data    dlgData;

static dyn_dim_type dynGetAutoSave( HWND hwndDlg, bool initial )
{
    initial = initial;
    if( IsDlgButtonChecked( hwndDlg, SETGEN_AUTOSAVE ) ) {
        return( DYN_VISIBLE );
    }
    return( DYN_DIM );
}

static bool dynIsAutoSave( WPARAM wparam, LPARAM lparam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    hwndDlg = hwndDlg;
#ifdef __NT__
    lparam = lparam;
#endif
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == SETGEN_AUTOSAVE && cmd == BN_CLICKED ) {
        return( true );
    }
    return( false );
}

static void gen_get( void *dlg, ctl_elt *ctl, void *data )
{
    switch( ctl->control ) {
    case SETGEN_AUTOSAVEINTERVAL:
        *(int *)data = ((dlg_data *)dlg)->AutoSaveInterval;
        break;
    case SETGEN_UNDO:
        *(bool *)data = ((dlg_data *)dlg)->Undo;
        break;
    case SETGEN_AUTOSAVE:
        *(bool *)data = ((dlg_data *)dlg)->AutoSave;
        break;
    case SETGEN_LASTEOL:
        *(bool *)data = ((dlg_data *)dlg)->LastEOL;
        break;
    case SETGEN_SAVECONFIG:
        *(bool *)data = ((dlg_data *)dlg)->SaveConfig;
        break;
    case SETGEN_SAVEONBUILD:
        *(bool *)data = ((dlg_data *)dlg)->SaveOnBuild;
        break;
    case SETGEN_BEEPFLAG:
        *(bool *)data = ((dlg_data *)dlg)->BeepFlag;
        break;
    case SETGEN_QUITMOVESFORWARD:
        *(bool *)data = ((dlg_data *)dlg)->QuitMovesForward;
        break;
    case SETGEN_SAMEFILECHECK:
        *(bool *)data = ((dlg_data *)dlg)->SameFileCheck;
        break;
    case SETGEN_MODAL:
        *(bool *)data = ((dlg_data *)dlg)->Modal;
        break;
    case SETGEN_CASEIGNORE:
        *(bool *)data = ((dlg_data *)dlg)->CaseIgnore;
        break;
    case SETGEN_SEARCHWRAP:
        *(bool *)data = ((dlg_data *)dlg)->SearchWrap;
        break;
    case SETGEN_WORD:
        *(char **)data = ((dlg_data *)dlg)->Word;
        break;
    case SETGEN_WORDALT:
        *(char **)data = ((dlg_data *)dlg)->WordAlt;
        break;
    case SETGEN_TMPDIR:
        *(char **)data = ((dlg_data *)dlg)->TmpDir;
        break;
    case SETGEN_HISTORYFILE:
        *(char **)data = ((dlg_data *)dlg)->HistoryFile;
        break;
    }
}

static void gen_set( void *dlg, ctl_elt *ctl, void *data )
{
    switch( ctl->control ) {
    case SETGEN_AUTOSAVEINTERVAL:
        ((dlg_data *)dlg)->AutoSaveInterval = *(int *)data;
        break;
    case SETGEN_UNDO:
        ((dlg_data *)dlg)->Undo = *(bool *)data;
        break;
    case SETGEN_AUTOSAVE:
        ((dlg_data *)dlg)->AutoSave = *(bool *)data;
        break;
    case SETGEN_LASTEOL:
        ((dlg_data *)dlg)->LastEOL = *(bool *)data;
        break;
    case SETGEN_SAVECONFIG:
        ((dlg_data *)dlg)->SaveConfig = *(bool *)data;
        break;
    case SETGEN_SAVEONBUILD:
        ((dlg_data *)dlg)->SaveOnBuild = *(bool *)data;
        break;
    case SETGEN_BEEPFLAG:
        ((dlg_data *)dlg)->BeepFlag = *(bool *)data;
        break;
    case SETGEN_QUITMOVESFORWARD:
        ((dlg_data *)dlg)->QuitMovesForward = *(bool *)data;
        break;
    case SETGEN_SAMEFILECHECK:
        ((dlg_data *)dlg)->SameFileCheck = *(bool *)data;
        break;
    case SETGEN_MODAL:
        ((dlg_data *)dlg)->Modal = *(bool *)data;
        break;
    case SETGEN_CASEIGNORE:
        ((dlg_data *)dlg)->CaseIgnore = *(bool *)data;
        break;
    case SETGEN_SEARCHWRAP:
        ((dlg_data *)dlg)->SearchWrap = *(bool *)data;
        break;
    case SETGEN_WORD:
        *(char **)data = ((dlg_data *)dlg)->Word;
        break;
    case SETGEN_WORDALT:
        *(char **)data = ((dlg_data *)dlg)->WordAlt;
        break;
    case SETGEN_TMPDIR:
        *(char **)data = ((dlg_data *)dlg)->TmpDir;
        break;
    case SETGEN_HISTORYFILE:
        *(char **)data = ((dlg_data *)dlg)->HistoryFile;
        break;
    }
}

#include "setgen.dh"
#include "setgen.ch"

static void globalTodlgData( void )
{
    strncpy( dlgData.Word, EditVars.WordDefn, WORDWIDTH - 1 );
    strncpy( dlgData.WordAlt, EditVars.WordAltDefn, WORDWIDTH - 1 );
    dlgData.Undo = EditFlags.Undo;
    dlgData.AutoSaveInterval = EditVars.AutoSaveInterval;
    dlgData.AutoSave = false;
    if( EditVars.AutoSaveInterval > 0 ) {
        dlgData.AutoSave = true;
    }
    dlgData.LastEOL = EditFlags.LastEOL;
    // dlgData.MaxLineLen = MaxLine;
    dlgData.SaveConfig = EditFlags.SaveConfig;
    dlgData.SaveOnBuild = EditFlags.SaveOnBuild;
    dlgData.BeepFlag = EditFlags.BeepFlag;
    dlgData.QuitMovesForward = EditFlags.QuitMovesForward;
    dlgData.SameFileCheck = EditFlags.SameFileCheck;
    dlgData.Modal = !EditFlags.Modeless;
    dlgData.CaseIgnore = EditFlags.CaseIgnore;
    dlgData.SearchWrap = EditFlags.SearchWrap;
    strncpy( dlgData.TmpDir, EditVars.TmpDir, TMPDIRWIDTH - 1 );
    strncpy( dlgData.HistoryFile, EditVars.HistoryFile, HISTORYFILEWIDTH - 1 );
}

static void dlgDataToGlobal( void )
{
    UtilUpdateStr( EditVars.WordDefn, dlgData.Word, "word" );
    UtilUpdateStr( EditVars.WordAltDefn, dlgData.WordAlt, "wordalt" );
    UtilUpdateBoolean( EditFlags.Undo, dlgData.Undo, "undo" );
    if( dlgData.AutoSave ) {
        UtilUpdateInt( EditVars.AutoSaveInterval, dlgData.AutoSaveInterval, "autosaveinterval" );
    } else {
        UtilUpdateInt( EditVars.AutoSaveInterval, 0, "autosaveinterval" );
    }
    UtilUpdateBoolean( EditFlags.LastEOL, dlgData.LastEOL, "lasteol" );
    // UtilUpdateInt( MaxLine, dlgData.MaxLineLen, "maxlinelen" );
    UtilUpdateBoolean( EditFlags.SaveConfig, dlgData.SaveConfig, "saveconfig" );
    UtilUpdateBoolean( EditFlags.SaveOnBuild, dlgData.SaveOnBuild, "saveonbuild" );
    UtilUpdateBoolean( EditFlags.BeepFlag, dlgData.BeepFlag, "beepflag" );
    UtilUpdateBoolean( EditFlags.QuitMovesForward, dlgData.QuitMovesForward, "quitmovesforward" );
    UtilUpdateBoolean( EditFlags.SameFileCheck, dlgData.SameFileCheck, "samefilecheck" );
    UtilUpdateBoolean( EditFlags.Modeless, !dlgData.Modal, "modeless" );
    UtilUpdateBoolean( EditFlags.CaseIgnore, dlgData.CaseIgnore, "caseignore" );
    UtilUpdateBoolean( EditFlags.SearchWrap, dlgData.SearchWrap, "searchwrap" );
    UtilUpdateStr( EditVars.TmpDir, dlgData.TmpDir, "tmpdir" );
    UtilUpdateStr( EditVars.HistoryFile, dlgData.HistoryFile, "historyfile" );
}

static void setdlgDataDefaults( void )
{
    // this sort of sucks since the default values aren't isolated in 1 place
    strncpy( dlgData.Word, WordDefnDefault + 6, WORDWIDTH - 1 );
    strncpy( dlgData.WordAlt, WordDefnDefault, WORDWIDTH - 1 );
    dlgData.Undo = true;
    dlgData.AutoSaveInterval = 30;
    dlgData.AutoSave = true;
    dlgData.LastEOL = false;
    // dlgData.MaxLineLen = 512;
    dlgData.SaveConfig = false;
    dlgData.SaveOnBuild = true;
    dlgData.BeepFlag = false;
    dlgData.QuitMovesForward = false;
    dlgData.SameFileCheck = true;
    dlgData.Modal = false;
    dlgData.CaseIgnore = true;
    dlgData.SearchWrap = true;
    strcpy( dlgData.TmpDir, "\\tmp" );
    strcpy( dlgData.HistoryFile, "\\vi.his" );
}

/*
 * SetGenProc - processes messages for the Data Control Dialog
 */
WINEXPORT BOOL CALLBACK SetGenProc( HWND hwndDlg, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwndDlg );
        globalTodlgData();
        ctl_dlg_init( GET_HINSTANCE( hwndDlg ), hwndDlg, &dlgData, &Ctl_setgen );
        dyn_tpl_init( &Dyn_setgen, hwndDlg );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case SETGEN_DEFAULTS:
            setdlgDataDefaults();
            ctl_dlg_reset( GET_HINSTANCE( hwndDlg ),
                           hwndDlg, &dlgData, &Ctl_setgen, TRUE );
            dyn_tpl_init( &Dyn_setgen, hwndDlg );
            return( TRUE );
        case IDOK:
            if( !ctl_dlg_done( GET_HINSTANCE( hwndDlg ),
                               hwndDlg, &dlgData, &Ctl_setgen ) ) {
                return( TRUE );
            }
            dlgDataToGlobal();

            // fall through
        case IDCANCEL:
            EndDialog( hwndDlg, TRUE );
            return( TRUE );
        }

        ctl_dlg_process( &Ctl_setgen, wparam, lparam );
        dyn_tpl_process( &Dyn_setgen, hwndDlg, wparam, lparam );
    }

    return( FALSE );
}

/*
 * GetSetGenDialog - create dialog box & get result
 */
bool GetSetGenDialog( void )
{
    FARPROC     proc;
    bool        rc;

    proc = MakeDlgProcInstance( SetGenProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SETGEN", root_window_id, (DLGPROC)proc );
    FreeProcInstance( proc );

    // redisplay all files to ensure screen completely correct
    ReDisplayBuffers( false );
    return( rc );

} /* GetSetGenDialog */
