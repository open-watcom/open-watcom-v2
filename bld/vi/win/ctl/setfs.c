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
* Description:  File specific settings dialog.
*
****************************************************************************/


#include "vi.h"
#include "setfs.rh"
#include "stddef.h"
#include "ctltype.h"
#include "dyntpl.h"
#include "fts.h"
#include "rcstr.grh"
#include <assert.h>
#include "wclbproc.h"
#include "winctl.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT INT_PTR CALLBACK SetFSDlgProc( HWND hwndDlg, UINT msg, WPARAM wparam, LPARAM lparam );

#define RC_VI_LANG_FIRST   RC_VI_LANG_LANG0
#define RC_VI_LANG_LAST    RC_VI_LANG_LANG0 + VI_LANG_MAX - 1

#define TAGFILENAMEWIDTH        129
#define GREPDEFAULTWIDTH        20
#define FT_TITLE                "File Specific Options ("

typedef struct {
    int         Language;
    int         TabAmount;
    int         HardTab;
    int         ShiftWidth;
    boolbit     PPKeywordOnly   : 1;
    boolbit     CMode           : 1;
    boolbit     ReadEntireFile  : 1;
    boolbit     ReadOnlyCheck   : 1;
    boolbit     IgnoreCtrlZ     : 1;
    boolbit     CRLFAutoDetect  : 1;
    boolbit     WriteCRLF       : 1;
    boolbit     EightBits       : 1;
    boolbit     RealTabs        : 1;
    boolbit     AutoIndent      : 1;
    boolbit     IgnoreTagCase   : 1;
    boolbit     TagPrompt       : 1;
    boolbit     ShowMatch       : 1;
    char        TagFileName[TAGFILENAMEWIDTH];
    char        GrepDefault[GREPDEFAULTWIDTH];
} dlg_data;

static dlg_data     *dlgDataArray = NULL;
static int          dlgDataArray_size = 0;

static dyn_dim_type dynGetLanguage( HWND hwndDlg, bool initial )
{
    int sel;

    initial = initial;
    sel = (int)SendDlgItemMessage( hwndDlg, SETFS_LANGUAGESELECT, CB_GETCURSEL, 0, 0L );
    switch( sel ) {
    case VI_LANG_NONE:
    case VI_LANG_USER:
        return( DYN_DIM );
    }
    return( DYN_VISIBLE );
}

static dyn_dim_type dynGetCRLFAutoDetect( HWND hwndDlg, bool initial )
{
    initial = initial;
    if( IsDlgButtonChecked( hwndDlg, SETFS_CRLFAUTODETECT ) ) {
        return( DYN_DIM );
    }
    return( DYN_VISIBLE );
}

static bool dynIsLanguage( WPARAM wparam, LPARAM lparam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    (void)hwndDlg;
#ifdef __NT__
    (void)lparam;
#endif
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == SETFS_LANGUAGESELECT && cmd == CBN_SELCHANGE ) {
        return( true );
    }
    return( false );
}

static bool dynIsCRLFAutoDetect( WPARAM wparam, LPARAM lparam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    (void)hwndDlg;
#ifdef __NT__
    (void)lparam;
#endif
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == SETFS_CRLFAUTODETECT && cmd == BN_CLICKED ) {
        return( true );
    }
    return( false );
}

static void fs_get( void *dlg, ctl_elt *ctl, void *data )
{
    switch( ctl->control ) {
    case SETFS_LANGUAGESELECT:
        *(int *)data = ((dlg_data *)dlg)->Language;
        break;
    case SETFS_TABAMOUNT:
        *(int *)data = ((dlg_data *)dlg)->TabAmount;
        break;
    case SETFS_HARDTAB:
        *(int *)data = ((dlg_data *)dlg)->HardTab;
        break;
    case SETFS_SHIFTWIDTH:
        *(int *)data = ((dlg_data *)dlg)->ShiftWidth;
        break;
    case SETFS_PPKEYWORDONLY:
        *(bool *)data = ((dlg_data *)dlg)->PPKeywordOnly;
        break;
    case SETFS_CMODE:
        *(bool *)data = ((dlg_data *)dlg)->CMode;
        break;
    case SETFS_READENTIREFILE:
        *(bool *)data = ((dlg_data *)dlg)->ReadEntireFile;
        break;
    case SETFS_READONLYCHECK:
        *(bool *)data = ((dlg_data *)dlg)->ReadOnlyCheck;
        break;
    case SETFS_IGNORECTRLZ:
        *(bool *)data = ((dlg_data *)dlg)->IgnoreCtrlZ;
        break;
    case SETFS_CRLFAUTODETECT:
        *(bool *)data = ((dlg_data *)dlg)->CRLFAutoDetect;
        break;
    case SETFS_WRITECRLF:
        *(bool *)data = ((dlg_data *)dlg)->WriteCRLF;
        break;
    case SETFS_EIGHTBITS:
        *(bool *)data = ((dlg_data *)dlg)->EightBits;
        break;
    case SETFS_REALTABS:
        *(bool *)data = ((dlg_data *)dlg)->RealTabs;
        break;
    case SETFS_AUTOINDENT:
        *(bool *)data = ((dlg_data *)dlg)->AutoIndent;
        break;
    case SETFS_IGNORETAGCASE:
        *(bool *)data = ((dlg_data *)dlg)->IgnoreTagCase;
        break;
    case SETFS_TAGPROMPT:
        *(bool *)data = ((dlg_data *)dlg)->TagPrompt;
        break;
    case SETFS_SHOWMATCH:
        *(bool *)data = ((dlg_data *)dlg)->ShowMatch;
        break;
    case SETFS_TAGFILENAME:
        *(char **)data = ((dlg_data *)dlg)->TagFileName;
        break;
    case SETFS_GREPDEFAULT:
        *(char **)data = ((dlg_data *)dlg)->GrepDefault;
        break;
    }
}

static void fs_set( void *dlg, ctl_elt *ctl, void *data )
{
    switch( ctl->control ) {
    case SETFS_LANGUAGESELECT:
        ((dlg_data *)dlg)->Language = *(int *)data;
        break;
    case SETFS_TABAMOUNT:
        ((dlg_data *)dlg)->TabAmount = *(int *)data;
        break;
    case SETFS_HARDTAB:
        ((dlg_data *)dlg)->HardTab = *(int *)data;
        break;
    case SETFS_SHIFTWIDTH:
        ((dlg_data *)dlg)->ShiftWidth = *(int *)data;
        break;
    case SETFS_PPKEYWORDONLY:
        ((dlg_data *)dlg)->PPKeywordOnly = *(bool *)data;
        break;
    case SETFS_CMODE:
        ((dlg_data *)dlg)->CMode = *(bool *)data;
        break;
    case SETFS_READENTIREFILE:
        ((dlg_data *)dlg)->ReadEntireFile = *(bool *)data;
        break;
    case SETFS_READONLYCHECK:
        ((dlg_data *)dlg)->ReadOnlyCheck = *(bool *)data;
        break;
    case SETFS_IGNORECTRLZ:
        ((dlg_data *)dlg)->IgnoreCtrlZ = *(bool *)data;
        break;
    case SETFS_CRLFAUTODETECT:
        ((dlg_data *)dlg)->CRLFAutoDetect = *(bool *)data;
        break;
    case SETFS_WRITECRLF:
        ((dlg_data *)dlg)->WriteCRLF = *(bool *)data;
        break;
    case SETFS_EIGHTBITS:
        ((dlg_data *)dlg)->EightBits = *(bool *)data;
        break;
    case SETFS_REALTABS:
        ((dlg_data *)dlg)->RealTabs = *(bool *)data;
        break;
    case SETFS_AUTOINDENT:
        ((dlg_data *)dlg)->AutoIndent = *(bool *)data;
        break;
    case SETFS_IGNORETAGCASE:
        ((dlg_data *)dlg)->IgnoreTagCase = *(bool *)data;
        break;
    case SETFS_TAGPROMPT:
        ((dlg_data *)dlg)->TagPrompt = *(bool *)data;
        break;
    case SETFS_SHOWMATCH:
        ((dlg_data *)dlg)->ShowMatch = *(bool *)data;
        break;
    case SETFS_TAGFILENAME:
        *(char **)data = ((dlg_data *)dlg)->TagFileName;
        break;
    case SETFS_GREPDEFAULT:
        *(char **)data = ((dlg_data *)dlg)->GrepDefault;
        break;
    }
}

#include "setfs.gdh"
#include "setfs.gch"

static void globalTodlgData( dlg_data *data, info *envInfo )
{
    if( envInfo != NULL ) {
        data->Language          = envInfo->fsi.Language;
        data->PPKeywordOnly     = EditFlags.PPKeywordOnly;
        data->CMode             = EditFlags.CMode;
        data->ReadEntireFile    = EditFlags.ReadEntireFile;
        data->ReadOnlyCheck     = EditFlags.ReadOnlyCheck;
        data->IgnoreCtrlZ       = EditFlags.IgnoreCtrlZ;
        data->CRLFAutoDetect    = EditFlags.CRLFAutoDetect;
        data->WriteCRLF         = EditFlags.WriteCRLF;
        data->EightBits         = EditFlags.EightBits;
        data->TabAmount         = EditVars.TabAmount;
        data->RealTabs          = EditFlags.RealTabs;
        data->HardTab           = EditVars.HardTab;
        data->AutoIndent        = EditFlags.AutoIndent;
        data->ShiftWidth        = EditVars.ShiftWidth;
        strncpy( data->TagFileName, EditVars.TagFileName, TAGFILENAMEWIDTH - 1 );
        data->TagFileName[TAGFILENAMEWIDTH - 1] = '\0';
        data->IgnoreTagCase     = EditFlags.IgnoreTagCase;
        data->TagPrompt         = EditFlags.TagPrompt;
        strncpy( data->GrepDefault, EditVars.GrepDefault, GREPDEFAULTWIDTH - 1 );
        data->GrepDefault[GREPDEFAULTWIDTH - 1] = '\0';
        data->ShowMatch         = EditFlags.ShowMatch;
    }
}

static void dlgDataDefault( dlg_data *data )
{
    data->Language       = VI_LANG_NONE;
    data->PPKeywordOnly  = false;
    data->CMode          = false;
    data->ReadEntireFile = false;
    data->ReadOnlyCheck  = true;
    data->IgnoreCtrlZ    = true;
    data->CRLFAutoDetect = true;
    data->WriteCRLF      = true;
    data->EightBits      = true;
    data->TabAmount      = 8;
    data->RealTabs       = false;
    data->HardTab        = 8;
    data->AutoIndent     = false;
    data->ShiftWidth     = 4;
    data->TagFileName[0] = '\0';
    data->IgnoreTagCase  = true;
    data->TagPrompt      = true;
    data->GrepDefault[0] = '\0';
    data->ShowMatch      = false;
}

static void dlgDataToGlobal( dlg_data *data )
{
    LangInit( data->Language );
    EditFlags.PPKeywordOnly     = data->PPKeywordOnly;
    EditFlags.CMode             = data->CMode;
    EditFlags.ReadEntireFile    = data->ReadEntireFile;
    EditFlags.ReadOnlyCheck     = data->ReadOnlyCheck;
    EditFlags.IgnoreCtrlZ       = data->IgnoreCtrlZ;
    EditFlags.CRLFAutoDetect    = data->CRLFAutoDetect;
    EditFlags.WriteCRLF         = data->WriteCRLF;
    EditFlags.EightBits         = data->EightBits;
    EditFlags.RealTabs          = data->RealTabs;
    EditFlags.AutoIndent        = data->AutoIndent;
    EditFlags.IgnoreTagCase     = data->IgnoreTagCase;
    EditFlags.TagPrompt         = data->TagPrompt;
    EditFlags.ShowMatch         = data->ShowMatch;
    EditVars.TabAmount          = data->TabAmount;
    EditVars.HardTab            = data->HardTab;
    EditVars.ShiftWidth         = data->ShiftWidth;

    /*
     * language specific variables with extra handling
     */
    if( strcmp( EditVars.TagFileName, data->TagFileName ) ) {
        ReplaceString( &EditVars.TagFileName, data->TagFileName );
    }
    if( strcmp( EditVars.GrepDefault, data->GrepDefault ) ) {
        ReplaceString( &EditVars.GrepDefault, data->GrepDefault );
    }
}

static void dlgDataInit( void )
{
    ft_src      *fts;

    dlgDataArray_size = 0;
    for( fts = FTSGetFirst(); fts != NULL; fts = FTSGetNext( fts ) ) {
        ++dlgDataArray_size;
    }
    dlgDataArray = _MemAllocArray( dlg_data, dlgDataArray_size );
}

static void dlgDataFini( void )
{
    MemFree( dlgDataArray );
}

static void filldlgData( dlg_data *data, const char *match, info *useInfo )
{
    // setup default values
    memset( useInfo, 0, sizeof( info ) );
    dlgDataDefault( data );
    dlgDataToGlobal( data );
    // run ftype commands (the wildcard should match itself ok!)
    FTSRunCmds( match );
    // copy environment
    globalTodlgData( data, useInfo );
}

static void fillFileType( HWND hwndDlg )
{
    HWND        hwndCB;
    ft_src      *fts;
    template_ll *template, *template1;
    char        str[_MAX_PATH];
    size_t      len;
    int         index;
    info        envInfo;
    info        *oldCurrentInfo;
    dlg_data    old_dlgData;

    // save vi setup, because this routine destroy it
    if( CurrentInfo != NULL ) {
        globalTodlgData( &old_dlgData, CurrentInfo );
    }
    oldCurrentInfo = CurrentInfo;
    CurrentInfo = &envInfo;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    for( index = 0, fts = FTSGetFirst(); fts != NULL; fts = FTSGetNext( fts ), ++index ) {
        str[0] = '\0';
        len = 0;
        template1 = FTSGetFirstTemplate( fts );
        for( template = template1; template != NULL; template = FTSGetNextTemplate( template ) ) {
            len += strlen( template->data ) + 2;
            if( len > sizeof( str ) ) {
                break;
            }
            strcat( str, " " );
            strcat( str, template->data );
        }
        filldlgData( dlgDataArray + index, template1->data, CurrentInfo );
        SendMessage( hwndCB, CB_INSERTSTRING, index, (LPARAM)(LPSTR)(str + 1) );
    }
    index = 0;
    if( oldCurrentInfo != NULL && oldCurrentInfo->CurrentFile != NULL ) {
        index = FTSSearchFTIndex( oldCurrentInfo->CurrentFile->name );
        if( index == -1 ) {
            index = 0;
        }
    }
    SendMessage( hwndCB, CB_SETCURSEL, index, 0L );

    // restore vi original setup
    LangFini( CurrentInfo->fsi.Language );
    CurrentInfo = oldCurrentInfo;
    if( CurrentInfo != NULL ) {
        dlgDataToGlobal( &old_dlgData );
    }
}

static void updateDialogSettings( HWND hwndDlg, bool title )
{
    HWND    hwndCB;
    int     index;
    char    *template;
    size_t  totallen;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    index = (int)SendMessage( hwndCB, CB_GETCURSEL, 0, 0L );

    if( title ) {
        totallen = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
        totallen += sizeof( FT_TITLE ) + 1;
        template = MemAlloc( totallen );
        strcpy( template, FT_TITLE );
        SendMessage( hwndCB, CB_GETLBTEXT, index, (LPARAM)(LPSTR)( template + sizeof( FT_TITLE ) - 1 ) );
        template[totallen - 2] = ')';
        template[totallen - 1] = '\0';
        SetWindowText( hwndDlg, template );
        MemFree( template );
    }

    ctl_dlg_init( GET_HINSTANCE( hwndDlg ), hwndDlg, dlgDataArray + index, &Ctl_setfs );
    dyn_tpl_init( &Dyn_setfs, hwndDlg );
}

static void dumpCommands( dlg_data *data )
{
    FTSAddBoolean( data->ReadEntireFile,    "readentirefile" );
    FTSAddBoolean( data->ReadOnlyCheck,     "readonlycheck" );
    FTSAddBoolean( data->IgnoreCtrlZ,       "ignorectrlz" );
    FTSAddBoolean( data->CRLFAutoDetect,    "crlfautodetect" );
    FTSAddBoolean( data->WriteCRLF,         "writecrlf" );
    FTSAddBoolean( data->EightBits,         "eightbits" );
    FTSAddBoolean( data->RealTabs,          "realtabs" );
    FTSAddBoolean( data->AutoIndent,        "autoindent" );
    FTSAddBoolean( data->IgnoreTagCase,     "ignoretagcase" );
    FTSAddBoolean( data->TagPrompt,         "tagprompt" );
    FTSAddBoolean( data->CMode,             "cmode" );
    FTSAddBoolean( data->ShowMatch,         "showmatch" );
    FTSAddBoolean( data->PPKeywordOnly,     "ppkeywordonly" );

    FTSAddInt( data->Language,              "language" );

    FTSAddInt( data->TabAmount,             "tabamount" );

    FTSAddInt( data->HardTab,               "hardtab" );
    FTSAddInt( data->ShiftWidth,            "shiftwidth" );

    FTSAddStr( data->TagFileName,           "tagfilename" );
    FTSAddStr( data->GrepDefault,           "grepdefault" );
}

static void writeSettings( HWND hwndDlg )
{
    // dump our little structure back into source line for fts
    HWND    hwndCB;
    int     dlgDataArray_count;
    int     index;
    char    *template;
    size_t  len;

    FTSFini();
    FTSInit();

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    dlgDataArray_count = (int)SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    for( index = 0; index < dlgDataArray_count; index++ ) {
        // put back in order we got them
        len = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
        template = MemAlloc( len + 1 );
        SendMessage( hwndCB, CB_GETLBTEXT, index, (LPARAM)(LPSTR)template );
        FTSStart( template );
        dumpCommands( dlgDataArray + index );
        FTSEnd();
        MemFree( template );
    }
    if( CurrentFile != NULL ) {
        FTSRunCmds( CurrentFile->name );
    }
}

static long deleteSelectedFT( HWND hwndDlg )
{
    HWND    hwndCB;
    size_t  len;
    int     rc;
    int     index;
    char    *template;
    int     dlgDataArray_count;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    dlgDataArray_count = (int)SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    index = (int)SendMessage( hwndCB, CB_GETCURSEL, 0, 0L );
    if( index == CB_ERR ) {
        MessageBox( hwndDlg, "No item selected", "", MB_ICONINFORMATION | MB_OK );
        return( 1L );
    }
    // get template in string form
    len = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
    template = MemAlloc( len + 1 );
    SendMessage( hwndCB, CB_GETLBTEXT, index, (LPARAM)(LPSTR)template );
    // can't delete *.* entry
    rc = IDYES;
    if( strcmp( template, "*.*" ) == 0 ) {
        MessageBox( hwndDlg, "Cannot delete catch-all item", "", MB_ICONINFORMATION | MB_OK );
        rc = IDNO;
    }
    if( rc == IDYES ) {
        // remove from our internal list
        memmove( dlgDataArray + index, dlgDataArray + index + 1, sizeof( dlg_data ) * ( dlgDataArray_count - index - 1 ) );
        // remove from Windows combo box
        SendMessage( hwndCB, CB_DELETESTRING, index, 0L );
        // reset selection
        if( index > 0 ) {
            index--;
        }
        SendMessage( hwndCB, CB_SETCURSEL, index, 0L );
        // update other dialog settings
        updateDialogSettings( GetParent( hwndCB ), true );
    }
    MemFree( template );
    return( 1L );
}

static long insertFT( HWND hwndDlg )
{
    HWND    hwndCB;
    char    *text;
    int     len;
    int     index;
    int     dlgDataArray_count;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    dlgDataArray_count = (int)SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    // get new template
    len = GetWindowTextLength( hwndCB );
    text = MemAlloc( len + 1 );
    GetWindowText( hwndCB, text, len + 1 );

    // attempt to insert at current position
    index = (int)SendMessage( hwndCB, CB_FINDSTRING, (WPARAM)-1L, (LPARAM)(LPSTR)text );
    if( index != CB_ERR ) {
        index = (int)SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
        if( index != CB_ERR && index == len ) {
            MessageBox( hwndDlg, "Template already defined", "", MB_ICONINFORMATION | MB_OK );
            return( 0L );
        }
    }
    // make memory space for new FT entry if necessary
    if( dlgDataArray_count + 1 > dlgDataArray_size ) {
        dlgDataArray_size++;
        dlgDataArray = _MemReallocArray( dlgDataArray, dlg_data, dlgDataArray_size );
    }
    // for now, always insert at top of list
    index = 0;
    memmove( dlgDataArray + index + 1, dlgDataArray + index, sizeof( dlg_data ) * ( dlgDataArray_count - index ) );
    dlgDataDefault( dlgDataArray + index );
    SendMessage( hwndCB, CB_INSERTSTRING, index, (LPARAM)(LPSTR)text );
    SendMessage( hwndCB, CB_SETCURSEL, index, 0L );
    updateDialogSettings( hwndDlg, true );

    return( 1L );
}

/*
 * SetFSDlgProc - processes messages for the Data Control Dialog
 */
WINEXPORT INT_PTR CALLBACK SetFSDlgProc( HWND hwndDlg, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         index;
    HWND        ctlhwnd;
    WORD        cmd;

    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwndDlg );
        dlgDataInit();
        fillFileType( hwndDlg );
        updateDialogSettings( hwndDlg, true );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case SETFS_FILETYPE:
            cmd = GET_WM_COMMAND_CMD( wparam, lparam );
            if( cmd == CBN_SELCHANGE ) {
//                rc = ctl_dlg_done( GET_HINSTANCE( hwndDlg ), hwndDlg, &oldData, &Ctl_setfs );
//                assert( rc != 0 );
                updateDialogSettings( hwndDlg, true );
            } else if( cmd == CBN_SETFOCUS ) {
                // if we are getting the focus, something probably changed
                ctlhwnd = GET_WM_COMMAND_HWND( wparam, lparam );
                index = (int)SendMessage( ctlhwnd, CB_GETCURSEL, 0, 0L );

                // this may bump focus back to a bad field
                if( ctl_dlg_done( GET_HINSTANCE( hwndDlg ), hwndDlg, dlgDataArray + index, &Ctl_setfs ) ) {
                    updateDialogSettings( hwndDlg, false );
                }
            }
            break;
        case SETFS_DELETE:
            deleteSelectedFT( hwndDlg );
            return( TRUE );
        case SETFS_INSERT:
            insertFT( hwndDlg );
            return( TRUE );
        case IDOK:
            index = (int)SendMessage( GetDlgItem( hwndDlg, SETFS_FILETYPE ), CB_GETCURSEL, 0, 0L );
            if( !ctl_dlg_done( GET_HINSTANCE( hwndDlg ), hwndDlg, dlgDataArray + index, &Ctl_setfs ) ) {
                return( TRUE );
            }
            writeSettings( hwndDlg );
            // fall through
        case IDCANCEL:
            dlgDataFini();
            EndDialog( hwndDlg, TRUE );
            return( TRUE );
        }

        ctl_dlg_process( &Ctl_setfs, wparam, lparam );
        dyn_tpl_process( &Dyn_setfs, hwndDlg, wparam, lparam );
    }
    return( FALSE );
}

/*
 * GetSetFSDialog - create dialog box & get result
 */
bool GetSetFSDialog( void )
{
    DLGPROC     dlgproc;
    bool        rc;

    dlgproc = MakeProcInstance_DLG( SetFSDlgProc, InstanceHandle );
    rc = ( DialogBox( InstanceHandle, "SETFS", root_window_id, dlgproc ) != 0 );
    FreeProcInstance_DLG( dlgproc );

    // redisplay all files to ensure screen completely correct
    ReDisplayBuffers( true );
    return( rc );

} /* GetSetFSDialog */
