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


#include <string.h>
#include "winvi.h"
#include "source.h"
#include "setfs.h"
#include "stddef.h"
#include "ctltype.h"
#include "dyntpl.h"
#include "fts.h"

#define TAGFILENAMEWIDTH        129
#define GREPDEFAULTWIDTH        20
#define FT_TITLE                "File Specific Options ("
#define MAX_FT_ENTRIES          20  // limit of setfs.c, not of fts.c


typedef struct {
    BOOL        LanguageBool;
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
    char        TagFileName[ TAGFILENAMEWIDTH ];
    BOOL        IgnoreTagCase;
    BOOL        TagPrompt;
    char        GrepDefault[ GREPDEFAULTWIDTH ];
    BOOL        ShowMatch;
} dlg_data;

static dlg_data     dlg_dataArray[ MAX_FT_ENTRIES ];

static dlg_data     cancelData;

static dyn_dim_type dynGetLanguage( HWND hwndDlg, BOOL initial )
{
    initial = initial;
    if( IsDlgButtonChecked( hwndDlg, SETFS_LANGUAGE ) ) {
        return( DYN_VISIBLE );
    }
    return( DYN_DIM );
}

static BOOL dynIsLanguage( UINT wParam, LONG lParam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    hwndDlg = hwndDlg;
    lParam = lParam;
    id = LOWORD( wParam );
    cmd = GET_WM_COMMAND_CMD( wParam, lParam );
    if( id == SETFS_LANGUAGE && cmd == BN_CLICKED ) {
        return( TRUE );
    }
    return( FALSE );
}

static dyn_dim_type dynGetLanguageButton( HWND hwndDlg, BOOL initial )
{
    initial = initial;
    if( IsDlgButtonChecked( hwndDlg, SETFS_LANGUAGE ) &&
        ( IsDlgButtonChecked( hwndDlg, SETFS_LANGUAGEC ) ||
          IsDlgButtonChecked( hwndDlg, SETFS_LANGUAGECPP ) ) ) {
        return( DYN_VISIBLE );
    }
    return( DYN_DIM );
}

static BOOL dynIsLanguageButton( UINT wParam, LONG lParam, HWND hwndDlg )
{
    WORD        id;
    WORD        cmd;

    hwndDlg = hwndDlg;
    lParam = lParam;
    id = LOWORD( wParam );
    cmd = GET_WM_COMMAND_CMD( wParam, lParam );
    if( ( id == SETFS_LANGUAGE ||
         ( id >= SETFS_LANGUAGEC && id <= SETFS_LANGUAGEFORTRAN ) ) &&
        cmd == BN_CLICKED ) {
        return( TRUE );
    }
    return( FALSE );
}

#include "setfs.dh"
#include "setfs.ch"

static void globalTodlg_data( dlg_data *data, info *envInfo )
{
    if( envInfo ){
        data->Language = envInfo->Language;
        data->LanguageBool = ( envInfo->Language > 0 ) ? TRUE : FALSE;
        data->PPKeywordOnly = EditFlags.PPKeywordOnly;
        data->CMode = EditFlags.CMode;
        data->ReadEntireFile = EditFlags.ReadEntireFile;
        data->ReadOnlyCheck = EditFlags.ReadOnlyCheck;
        data->IgnoreCtrlZ = EditFlags.IgnoreCtrlZ;
        data->CRLFAutoDetect = EditFlags.CRLFAutoDetect;
        data->WriteCRLF = EditFlags.WriteCRLF;
        data->EightBits = EditFlags.EightBits;
        data->TabAmount = TabAmount;
        data->RealTabs = EditFlags.RealTabs;
        data->HardTab = HardTab;
        data->AutoIndent = EditFlags.AutoIndent;
        data->ShiftWidth = ShiftWidth;
        strncpy( data->TagFileName, TagFileName, TAGFILENAMEWIDTH - 1 );
        data->TagFileName[ TAGFILENAMEWIDTH - 1 ] = '\0';
        data->IgnoreTagCase = EditFlags.IgnoreTagCase;
        data->TagPrompt = EditFlags.TagPrompt;
        strncpy( data->GrepDefault, GrepDefault, GREPDEFAULTWIDTH - 1 );
        data->GrepDefault[ GREPDEFAULTWIDTH - 1 ] = '\0';
        data->ShowMatch = EditFlags.ShowMatch;
    }
}

static void dlg_dataDefault( dlg_data *data )
{
    data->Language = 1;
    data->LanguageBool = FALSE;
    data->PPKeywordOnly = FALSE;
    data->CMode = FALSE;
    data->ReadEntireFile = FALSE;
    data->ReadOnlyCheck = TRUE;
    data->IgnoreCtrlZ = TRUE;
    data->CRLFAutoDetect = TRUE;
    data->WriteCRLF = TRUE;
    data->EightBits = TRUE;
    data->TabAmount = 8;
    data->RealTabs = FALSE;
    data->HardTab = 8;
    data->AutoIndent = FALSE;
    data->ShiftWidth = 4;
    data->TagFileName[ 0 ] = '\0';
    data->IgnoreTagCase = TRUE;
    data->TagPrompt = TRUE;
    data->GrepDefault[ 0 ] = '\0';
    data->ShowMatch = FALSE;
}

static void filldlg_dataArray( int index, char *match, info *useInfo )
{
    // run ftype commands (the wildcard should match itself ok!)
    FTSRunCmds( match );

    // copy environment
    globalTodlg_data( &( dlg_dataArray[ index ] ), useInfo );
}

void fillFileType( HWND hwndDlg )
{
    HWND        hwndCB;
    ft_src      *fts;
    template_ll *template, *template1;
    char        str[ _MAX_PATH ];
    int         strLen;
    int         index;
    info        envInfo, *oldCurrentInfo;

    oldCurrentInfo = CurrentInfo;
    if( CurrentInfo == NULL ) {
        memset( &envInfo, 0, sizeof( envInfo ) );
        CurrentInfo = &envInfo;
    }

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    fts = FTSGetFirst();
    index = 0;
    while( fts ) {
        template1 = template = FTSGetFirstTemplate( fts );
        str[ 0 ] = '\0';
        strLen = 0;
        while( template ) {
            strLen += strlen( template->data ) + 2;
            if( strLen > sizeof( str ) ) break;
            strcat( str, " " );
            strcat( str, template->data );
            template = FTSGetNextTemplate( template );
        }
        filldlg_dataArray( index, template1->data, CurrentInfo );
        SendMessage( hwndCB, CB_INSERTSTRING, index, (LPARAM)( str + 1 ) );
        index++;
        fts = FTSGetNext( fts );
    }
    SendMessage( hwndCB, CB_SETCURSEL, 0, 0L );

    CurrentInfo = oldCurrentInfo;
}

void updateDialogSettings( HWND hwndDlg, BOOL title )
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
        SendMessage( hwndCB, CB_GETLBTEXT, index,
                     (LONG)( template + sizeof( FT_TITLE ) - 1 ) );
        template[ totallen - 2 ] = ')';
        template[ totallen - 1 ] = '\0';
        SetWindowText( hwndDlg, template );
        MemFree( template );
    }

    ctl_dlg_init( GET_HINSTANCE( hwndDlg ), hwndDlg,
                   &( dlg_dataArray[ index ] ), &Ctl_setfs );
    dyn_tpl_init( &Dyn_setfs, hwndDlg );
}

void dumpCommands( int i )
{
    FTSAddBoolean( dlg_dataArray[ i ].ReadEntireFile, "readentirefile" );
    FTSAddBoolean( dlg_dataArray[ i ].ReadOnlyCheck, "readonlycheck" );
    FTSAddBoolean( dlg_dataArray[ i ].IgnoreCtrlZ, "ignorectrlz" );
    FTSAddBoolean( dlg_dataArray[ i ].CRLFAutoDetect, "crlfautodetect" );
    FTSAddBoolean( dlg_dataArray[ i ].WriteCRLF, "writecrlf" );
    FTSAddBoolean( dlg_dataArray[ i ].EightBits, "eightbits" );
    FTSAddBoolean( dlg_dataArray[ i ].RealTabs, "realtabs" );
    FTSAddBoolean( dlg_dataArray[ i ].AutoIndent, "autoindent" );
    FTSAddBoolean( dlg_dataArray[ i ].IgnoreTagCase, "ignoretagcase" );
    FTSAddBoolean( dlg_dataArray[ i ].TagPrompt, "tagprompt" );
    FTSAddBoolean( dlg_dataArray[ i ].CMode, "cmode" );
    FTSAddBoolean( dlg_dataArray[ i ].ShowMatch, "showmatch" );
    FTSAddBoolean( dlg_dataArray[ i ].PPKeywordOnly, "ppkeywordonly" );

    if( !dlg_dataArray[ i ].LanguageBool ) {
        FTSAddInt( 0, "language" );
    } else {
        FTSAddInt( dlg_dataArray[ i ].Language, "language" );
    }
    FTSAddInt( dlg_dataArray[ i ].TabAmount, "tabamount" );

    FTSAddInt( dlg_dataArray[ i ].HardTab, "hardtab" );
    FTSAddInt( dlg_dataArray[ i ].ShiftWidth, "shiftwidth" );

    FTSAddStr( dlg_dataArray[ i ].TagFileName, "tagfilename" );
    FTSAddStr( dlg_dataArray[ i ].GrepDefault, "grepdefault" );
}

void writeSettings( HWND hwndDlg )
{
    // dump our little structure back into source line for fts
    HWND    hwndCB;
    int     nTemplates, i;
    char    *template;
    int     len;

    FTSFini();
    FTSInit();

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    nTemplates = SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    for( i = 0; i < nTemplates; i++ ) {
        // put back in order we got them
        len = SendMessage( hwndCB, CB_GETLBTEXTLEN, i, 0L );
        template = MemAlloc( len + 1 );
        SendMessage( hwndCB, CB_GETLBTEXT, i, (LONG)template );
        FTSStart( template );
        dumpCommands( i );
        FTSEnd();
        MemFree( template );
    }
    if( CurrentFile ){
        FTSRunCmds( CurrentFile->name );
    }
}

long deleteSelectedFT( HWND hwndDlg )
{
    HWND    hwndCB;
    int     i, len, rc;
    char    *template;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );
    i = SendMessage( hwndCB, CB_GETCURSEL, 0, 0L );
    if( i == CB_ERR ) {
        MessageBox( hwndDlg, "No item selected",
                     "", MB_ICONINFORMATION | MB_OK );
        return( 1L );
    }

    // get template in string form
    len = SendMessage( hwndCB, CB_GETLBTEXTLEN, i, 0L );
    template = MemAlloc( len + 1 );
    SendMessage( hwndCB, CB_GETLBTEXT, i, (LONG)template );

    // can't delete *.* entry
    rc = IDYES;
    if( !strcmp( template, "*.*" ) ) {
        MessageBox( hwndDlg, "Cannot delete catch-all item",
                     "", MB_ICONINFORMATION | MB_OK );
        rc = IDNO;
    }

    if( rc == IDYES ) {
        // remove from our internal list
        memmove( dlg_dataArray + i, dlg_dataArray + i + 1,
                 sizeof( dlg_data ) * ( MAX_FT_ENTRIES - i - 1 ) );

        // remove from Windows combo box
        SendMessage( hwndCB, CB_DELETESTRING, i, 0l );

        // reset selection
        if( i > 0 ) {
            i--;
        }
        SendMessage( hwndCB, CB_SETCURSEL, i, 0L );

        // update other dialog settings
        updateDialogSettings( GetParent( hwndCB ), TRUE );
    }
    MemFree( template );
    return( 1L );
}

long insertFT( HWND hwndDlg )
{
    HWND    hwndCB;
    char    *text;
    int     len, i;
    int     nTemplates;

    hwndCB = GetDlgItem( hwndDlg, SETFS_FILETYPE );

    // get new template
    len = GetWindowTextLength( hwndCB );
    text = MemAlloc( len + 1 );
    GetWindowText( hwndCB, text, len + 1 );
    nTemplates = SendMessage( hwndCB, CB_GETCOUNT, 0, 0L );

    // attempt to insert at current position
    i = SendMessage( hwndCB, CB_FINDSTRING, -1, (LONG)text );
    if( i != CB_ERR &&
        SendMessage( hwndCB, CB_GETLBTEXTLEN, i, 0L ) == strlen( text ) ) {
        MessageBox( hwndDlg, "Template already defined",
                     "", MB_ICONINFORMATION | MB_OK );
        return( 0L );
    }
    if( nTemplates == MAX_FT_ENTRIES ) {
        MessageBox( hwndDlg, "Cannot hold any more templates",
                     "", MB_ICONINFORMATION | MB_OK );
        return( 0L );
    }
    // for now, always insert at top of list
    i = 0;
    memmove( dlg_dataArray + i + 1, dlg_dataArray + i,
             sizeof( dlg_data ) * ( MAX_FT_ENTRIES - i - 1 ) );
    dlg_dataDefault( &dlg_dataArray[ i ] );
    SendMessage( hwndCB, CB_INSERTSTRING, i, (LONG)text );
    SendMessage( hwndCB, CB_SETCURSEL, i, 0L );
    updateDialogSettings( hwndDlg, TRUE );

    return( 1L );
}

BOOL WINEXP EnumChildProc( HWND hwndChild, DWORD lParam )
{
    char    szClass[ 5 ];
    lParam = lParam;
    GetClassName( hwndChild, szClass, 5 );
    if( !strcmp( szClass, "Edit" ) ) {
        //hwndEdit = hwndChild;
        return( FALSE );
    }
    return( TRUE );
}

/*
 * SetFSProc - processes messages for the Data Control Dialog
 */
BOOL WINEXP SetFSProc( HWND hwndDlg, unsigned msg,
                       UINT wParam, LONG lParam )
{
    dlg_data    oldData;        // junk
    int         rc;
    int         index;
    HWND        ctlhwnd;
    WORD        cmd;

    switch( msg ) {
    case WM_INITDIALOG:
        EditFlags.Quiet = TRUE;
        globalTodlg_data( &cancelData, CurrentInfo );
        CenterWindowInRoot( hwndDlg );
        fillFileType( hwndDlg );
        updateDialogSettings( hwndDlg, TRUE );
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case SETFS_FILETYPE:
            cmd = GET_WM_COMMAND_CMD( wParam, lParam );
            if( cmd == CBN_SELCHANGE ) {
                rc = ctl_dlg_done( GET_HINSTANCE( hwndDlg ),
                                   hwndDlg, &oldData, &Ctl_setfs );
                assert( rc != 0 );
                updateDialogSettings( hwndDlg, TRUE );
            } else if( cmd == CBN_SETFOCUS ) {
                // if we are getting the focus, something probably changed
                ctlhwnd = GET_WM_COMMAND_HWND( wParam, lParam );
                index = SendMessage( ctlhwnd, CB_GETCURSEL, 0, 0L );

                // this may bump focus back to a bad field
                if( ctl_dlg_done( GET_HINSTANCE( hwndDlg ),
                        hwndDlg, &( dlg_dataArray[ index ] ), &Ctl_setfs ) ) {
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
            index = SendMessage( GetDlgItem( hwndDlg, SETFS_FILETYPE ),
                                 CB_GETCURSEL, 0, 0L );
            if( !ctl_dlg_done( GET_HINSTANCE( hwndDlg ),
                               hwndDlg, &( dlg_dataArray[ index ] ),
                               &Ctl_setfs ) ) {
                return( TRUE );
            }
            writeSettings( hwndDlg );
            EndDialog( hwndDlg, TRUE );
            EditFlags.Quiet = FALSE;
            return( TRUE );
        case IDCANCEL:
            if( CurrentInfo ){
                cancelSettings();
            }
            EndDialog( hwndDlg, TRUE );
            EditFlags.Quiet = FALSE;
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

    proc = (DLGPROC) MakeProcInstance( (FARPROC) SetFSProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SETFS", Root, proc );
    FreeProcInstance( (FARPROC) proc );

    // redisplay all files to ensure screen completely correct
    ReDisplayBuffers( TRUE );
    return( rc );
} /* GetSetFSDialog */

static void cancelSettings( void ){
    CurrentInfo->Language       = cancelData.Language;
    EditFlags.PPKeywordOnly     = cancelData.PPKeywordOnly;
    EditFlags.CMode             = cancelData.CMode;
    EditFlags.ReadEntireFile    = cancelData.ReadEntireFile;
    EditFlags.ReadOnlyCheck     = cancelData.ReadOnlyCheck;
    EditFlags.IgnoreCtrlZ       = cancelData.IgnoreCtrlZ;
    EditFlags.CRLFAutoDetect    = cancelData.CRLFAutoDetect;
    EditFlags.WriteCRLF         = cancelData.WriteCRLF;
    EditFlags.EightBits         = cancelData.EightBits;
    EditFlags.RealTabs          = cancelData.RealTabs;
    EditFlags.AutoIndent        = cancelData.AutoIndent;
    EditFlags.IgnoreTagCase     = cancelData.IgnoreTagCase;
    EditFlags.TagPrompt         = cancelData.TagPrompt;
    EditFlags.ShowMatch         = cancelData.ShowMatch;
    TabAmount                   = cancelData.TabAmount;
    HardTab                     = cancelData.HardTab;
    ShiftWidth                  = cancelData.ShiftWidth;

    // strncpy( cancelData->TagFileName, TagFileName, TAGFILENAMEWIDTH - 1 );
    // GrepDefault = cancelData->GrepDefault[ GREPDEFAULTWIDTH - 1 ] = '\0';
}
