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
* Description:  File specific settings dialog.
*
****************************************************************************/


#include "vi.h"
#include "source.h"
#include "setfs.h"
#include "stddef.h"
#include "ctltype.h"
#include "dyntpl.h"
#include "fts.h"
#include "rcstr.gh"
#include <assert.h>

#define VI_LANG_FIRST   VI_LANG_LANG0
#define VI_LANG_LAST    VI_LANG_LANG0 + LANG_MAX - 1

#define TAGFILENAMEWIDTH        129
#define GREPDEFAULTWIDTH        20
#define FT_TITLE                "File Specific Options ("


typedef struct {
    int         Language;
    BOOL        PPKeywordOnly;
    BOOL        CMode;
    BOOL        ReadEntireFile;
    BOOL        ReadOnlyCheck;
    BOOL        IgnoreCtrlZ;
    BOOL        CRLFAutoDetect;
    BOOL        WriteCRLF;
    BOOL        EightBits;
    int         TabAmount;
    BOOL        RealTabs;
    int         HardTab;
    BOOL        AutoIndent;
    int         ShiftWidth;
    char        TagFileName[TAGFILENAMEWIDTH];
    BOOL        IgnoreTagCase;
    BOOL        TagPrompt;
    char        GrepDefault[GREPDEFAULTWIDTH];
    BOOL        ShowMatch;
} dlg_data;

static dlg_data     *dlgDataArray = NULL;
static int          dlgDataArray_size = 0;

static dyn_dim_type dynGetLanguage( HWND hwndDlg, BOOL initial )
{
    int sel;
    
    initial = initial;
    sel = (int)SendDlgItemMessage( hwndDlg, SETFS_LANGUAGESELECT, CB_GETCURSEL, 0, 0L );
    switch( sel ) {
    case LANG_NONE:
    case LANG_USER:
        return( DYN_DIM );
    }
    return( DYN_VISIBLE );
}

static BOOL dynIsLanguage( UINT wParam, LONG lParam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    hwndDlg = hwndDlg;
    lParam = lParam;
    id = LOWORD( wParam );
    cmd = GET_WM_COMMAND_CMD( wParam, lParam );
    if( id == SETFS_LANGUAGESELECT && cmd == CBN_SELCHANGE ) {
        return( TRUE );
    }
    return( FALSE );
}

#include "setfs.dh"
#include "setfs.ch"

static void globalTodlgData( dlg_data *data, info *envInfo )
{
    if( envInfo != NULL ) {
        data->Language          = envInfo->Language;
        data->PPKeywordOnly     = EditFlags.PPKeywordOnly;
        data->CMode             = EditFlags.CMode;
        data->ReadEntireFile    = EditFlags.ReadEntireFile;
        data->ReadOnlyCheck     = EditFlags.ReadOnlyCheck;
        data->IgnoreCtrlZ       = EditFlags.IgnoreCtrlZ;
        data->CRLFAutoDetect    = EditFlags.CRLFAutoDetect;
        data->WriteCRLF         = EditFlags.WriteCRLF;
        data->EightBits         = EditFlags.EightBits;
        data->TabAmount         = TabAmount;
        data->RealTabs          = EditFlags.RealTabs;
        data->HardTab           = HardTab;
        data->AutoIndent        = EditFlags.AutoIndent;
        data->ShiftWidth        = ShiftWidth;
        strncpy( data->TagFileName, TagFileName, TAGFILENAMEWIDTH - 1 );
        data->TagFileName[TAGFILENAMEWIDTH - 1] = '\0';
        data->IgnoreTagCase     = EditFlags.IgnoreTagCase;
        data->TagPrompt         = EditFlags.TagPrompt;
        strncpy( data->GrepDefault, GrepDefault, GREPDEFAULTWIDTH - 1 );
        data->GrepDefault[GREPDEFAULTWIDTH - 1] = '\0';
        data->ShowMatch         = EditFlags.ShowMatch;
    }
}

static void dlgDataDefault( dlg_data *data )
{
    data->Language       = LANG_NONE;
    data->PPKeywordOnly  = FALSE;
    data->CMode          = FALSE;
    data->ReadEntireFile = FALSE;
    data->ReadOnlyCheck  = TRUE;
    data->IgnoreCtrlZ    = TRUE;
    data->CRLFAutoDetect = TRUE;
    data->WriteCRLF      = TRUE;
    data->EightBits      = TRUE;
    data->TabAmount      = 8;
    data->RealTabs       = FALSE;
    data->HardTab        = 8;
    data->AutoIndent     = FALSE;
    data->ShiftWidth     = 4;
    data->TagFileName[0] = '\0';
    data->IgnoreTagCase  = TRUE;
    data->TagPrompt      = TRUE;
    data->GrepDefault[0] = '\0';
    data->ShowMatch      = FALSE;
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
    TabAmount                   = data->TabAmount;
    HardTab                     = data->HardTab;
    ShiftWidth                  = data->ShiftWidth;

    /*
     * language specific variables with extra handling
     */
    if( strcmp( TagFileName, data->TagFileName ) ) {
        AddString2( &TagFileName, data->TagFileName );
    }
    if( strcmp( GrepDefault, data->GrepDefault ) ) {
        AddString2( &GrepDefault, data->GrepDefault );
    }
}

static void dlgDataInit( void )
{
    ft_src      *fts;

    dlgDataArray_size = 0;
    for( fts = FTSGetFirst(); fts != NULL; fts = FTSGetNext( fts ) ) {
        ++dlgDataArray_size;
    }
    dlgDataArray = MemAlloc( dlgDataArray_size * sizeof( dlg_data ) );
}

static void dlgDataFini( void )
{
    MemFree( dlgDataArray );
}

static void filldlgData( dlg_data *data, char *match, info *useInfo )
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
    int         strLen;
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
        template1 = template = FTSGetFirstTemplate( fts );
        str[0] = '\0';
        strLen = 0;
        while( template ) {
            strLen += strlen( template->data ) + 2;
            if( strLen > sizeof( str ) ) {
                break;
            }
            strcat( str, " " );
            strcat( str, template->data );
            template = FTSGetNextTemplate( template );
        }
        filldlgData( dlgDataArray + index, template1->data, CurrentInfo );
        SendMessage( hwndCB, CB_INSERTSTRING, index, (LPARAM)(str + 1) );
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
    LangFini( CurrentInfo->Language );
    CurrentInfo = oldCurrentInfo;
    if( CurrentInfo != NULL ) {
        dlgDataToGlobal( &old_dlgData );
    }
}

static void updateDialogSettings( HWND hwndDlg, BOOL title )
{
    HWND    hwndCB;
    int     index;
    char    *template;
    int     totallen;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    index = SendMessage( hwndCB, CB_GETCURSEL, 0, 0L );

    if( title ) {
        totallen = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
        totallen += sizeof( FT_TITLE ) + 1;
        template = MemAlloc( totallen );
        strcpy( template, FT_TITLE );
        SendMessage( hwndCB, CB_GETLBTEXT, index, (LONG)(template + sizeof( FT_TITLE ) - 1) );
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
    int     len;

    FTSFini();
    FTSInit();

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    dlgDataArray_count = SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    for( index = 0; index < dlgDataArray_count; index++ ) {
        // put back in order we got them
        len = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
        template = MemAlloc( len + 1 );
        SendMessage( hwndCB, CB_GETLBTEXT, index, (LONG)template );
        FTSStart( template );
        dumpCommands( dlgDataArray + index );
        FTSEnd();
        MemFree( template );
    }
    if( CurrentFile ) {
        FTSRunCmds( CurrentFile->name );
    }
}

static long deleteSelectedFT( HWND hwndDlg )
{
    HWND    hwndCB;
    int     len, rc;
    int     index;
    char    *template;
    int     dlgDataArray_count;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    dlgDataArray_count = SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    index = SendMessage( hwndCB, CB_GETCURSEL, 0, 0L );
    if( index == CB_ERR ) {
        MessageBox( hwndDlg, "No item selected", "", MB_ICONINFORMATION | MB_OK );
        return( 1L );
    }
    // get template in string form
    len = SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L );
    template = MemAlloc( len + 1 );
    SendMessage( hwndCB, CB_GETLBTEXT, index, (LONG)template );
    // can't delete *.* entry
    rc = IDYES;
    if( !strcmp( template, "*.*" ) ) {
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
        updateDialogSettings( GetParent( hwndCB ), TRUE );
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
    dlgDataArray_count = SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    // get new template
    len = GetWindowTextLength( hwndCB );
    text = MemAlloc( len + 1 );
    GetWindowText( hwndCB, text, len + 1 );

    // attempt to insert at current position
    index = SendMessage( hwndCB, CB_FINDSTRING, -1, (LONG)text );
    if( index != CB_ERR && SendMessage( hwndCB, CB_GETLBTEXTLEN, index, 0L ) == strlen( text ) ) {
        MessageBox( hwndDlg, "Template already defined", "", MB_ICONINFORMATION | MB_OK );
        return( 0L );
    }
    // make memory space for new FT entry if necessary
    if( dlgDataArray_count + 1 > dlgDataArray_size ) {
        dlgDataArray = MemReAlloc( dlgDataArray, ( ++dlgDataArray_size ) * sizeof( dlg_data ) );
    }
    // for now, always insert at top of list
    index = 0;
    memmove( dlgDataArray + index + 1, dlgDataArray + index, sizeof( dlg_data ) * ( dlgDataArray_count - index ) );
    dlgDataDefault( dlgDataArray + index );
    SendMessage( hwndCB, CB_INSERTSTRING, index, (LONG)text );
    SendMessage( hwndCB, CB_SETCURSEL, index, 0L );
    updateDialogSettings( hwndDlg, TRUE );

    return( 1L );
}

/*
 * SetFSProc - processes messages for the Data Control Dialog
 */
BOOL WINEXP SetFSProc( HWND hwndDlg, unsigned msg, UINT wParam, LONG lParam )
{
    int         index;
    HWND        ctlhwnd;
    WORD        cmd;

    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwndDlg );
        dlgDataInit();
        fillFileType( hwndDlg );
        updateDialogSettings( hwndDlg, TRUE );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case SETFS_FILETYPE:
            cmd = GET_WM_COMMAND_CMD( wParam, lParam );
            if( cmd == CBN_SELCHANGE ) {
//                rc = ctl_dlg_done( GET_HINSTANCE( hwndDlg ), hwndDlg, &oldData, &Ctl_setfs );
//                assert( rc != 0 );
                updateDialogSettings( hwndDlg, TRUE );
            } else if( cmd == CBN_SETFOCUS ) {
                // if we are getting the focus, something probably changed
                ctlhwnd = GET_WM_COMMAND_HWND( wParam, lParam );
                index = SendMessage( ctlhwnd, CB_GETCURSEL, 0, 0L );

                // this may bump focus back to a bad field
                if( ctl_dlg_done( GET_HINSTANCE( hwndDlg ), hwndDlg, dlgDataArray + index, &Ctl_setfs ) ) {
                    updateDialogSettings( hwndDlg, FALSE );
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
            index = SendMessage( GetDlgItem( hwndDlg, SETFS_FILETYPE ), CB_GETCURSEL, 0, 0L );
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

        ctl_dlg_process( &Ctl_setfs, wParam, lParam );
        dyn_tpl_process( &Dyn_setfs, hwndDlg, wParam, lParam );
    }
    return( FALSE );
}

/*
 * GetSetFSDialog - create dialog box & get result
 */
bool GetSetFSDialog( void )
{
    DLGPROC     proc;
    bool        rc;

    proc = (DLGPROC)MakeProcInstance( (FARPROC)SetFSProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SETFS", Root, proc );
    FreeProcInstance( (FARPROC)proc );

    // redisplay all files to ensure screen completely correct
    ReDisplayBuffers( TRUE );
    return( rc );

} /* GetSetFSDialog */
