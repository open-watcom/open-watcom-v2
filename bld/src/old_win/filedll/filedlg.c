#include <windows.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <errno.h>
#include "filedlg.h"
#include "filerc.h"

HINSTANCE       Inst;
LPFILEOPEN      Ofs;
char            HasFile;
char            _ext[128];
char            Buff[256];

/*
 * LibMain - main DLL entry point
 */
int FAR PASCAL LibMain( HINSTANCE inst, WORD data, WORD heapsize,
                            LPSTR cmdline )
{
    cmdline = cmdline;
    data = data;
    heapsize = heapsize;

    Inst = inst;
    return( 1 );

} /* LibMain */

/*
 * WEP - dll is exiting
 */
int __export FAR PASCAL WEP( int sys_exit )
{
    sys_exit = sys_exit;
    return( 1 );

} /* WEP */


/*
 * CheckSaveVerify - see if save-verify request is needed
 */
static BOOL CheckSaveVerify( void )
{
    WORD        rc;

    if( Ofs->type == FILE_SAVE_VERIFY ) {
        rc = MessageBox( NULL,Buff,"Really overwrite?",
                MB_OKCANCEL | MB_TASKMODAL );
        if( rc == IDOK ) return( TRUE );
        return( FALSE );
    }
    return( TRUE );

} /* CheckSaveVerify */

/*
 * BuildDirectory - build the directory boxes
 */
static void BuildDirectory( HWND hwnd )
{
    WORD        handle;

    SendDlgItemMessage( hwnd, DRIVE_BOX, LB_RESETCONTENT, 0, 0L );
    SendDlgItemMessage( hwnd, FILE_BOX, LB_RESETCONTENT, 0, 0L );
    SendDlgItemMessage( hwnd, DIR_BOX, LB_RESETCONTENT, 0, 0L );
    DlgDirList( hwnd, _ext, DRIVE_BOX, NULL, 0x8000 | 0x4000 );
    DlgDirList( hwnd, _ext, FILE_BOX, NULL, 0x0000 );
    DlgDirList( hwnd, _ext, DIR_BOX, NULL, 0x8000 | 0x0010 );
    handle = GetDlgItem( hwnd, FILE_EDIT );
    SetWindowText( handle, (LPSTR) _ext );
    handle = GetDlgItem( hwnd, FILE_CWD );
    getcwd( Buff, _MAX_PATH );
    SetWindowText( handle, (LPSTR) Buff );

} /* BuildDirectory */

/*
 * TryTheFile - try the current file
 */
static BOOL TryTheFile( HWND hwnd )
{
    char        tmp[_MAX_PATH];
    char        drive[_MAX_DRIVE],directory[_MAX_DIR];
    char        name[_MAX_FNAME],ext[_MAX_EXT];
    unsigned    total;
    int         rc;
    int         h;
    int         i;
    int         len;
    BOOL        isdir = FALSE;
    BOOL        iswild = FALSE;

    GetDlgItemText( hwnd, FILE_EDIT, (LPSTR) Buff, _MAX_PATH );
    rc = access( Buff, ACCESS_RD );
    if( !rc ) {
        /*
         * see if this is a directory
         */
        strcpy( tmp,Buff );
        if( tmp[strlen(tmp)-1] != '\\' ) strcat( tmp,"\\" );
        strcat( tmp,"NUL" );
        h = open( tmp, O_RDONLY );
        if( h < 0 ) {
            return( CheckSaveVerify() );
        }
        close( h );
        isdir = TRUE;
    }
    len = strlen( Buff );
    for( i=0; i<len; i++ ) {
        if( Buff[i] == '*' || Buff[i] == '?' ) {
            iswild = TRUE;
            break;
        }
    }
    if( errno == ENOENT && !iswild ) return( TRUE );

    _splitpath( Buff, drive, directory, name, ext );
    if( drive[0] != 0 ) {
        _dos_setdrive( tolower( drive[0] ) - 'a' + 1, &total );
    }
    if( isdir ) {
        strcat( directory, name );
        strcat( directory, ext );
        name[0] = 0;
        ext[0] = 0;
    }
    if( directory[0] != 0 ) {
        i = strlen( directory );
        if( directory[i-1] == '\\' ) directory[i-1] = 0;
        chdir( directory );
    }
    if( name[0] == '*' || ext[1] == '*' ) {
        strcpy( _ext, name );
        strcat( _ext, ext );
    } else {
        strcpy( _ext, Ofs->ext );
    }
    BuildDirectory( hwnd );
    return( FALSE );

} /* TryTheFile */

/*
 * GetFile - get file dialog
 */
BOOL __export FAR PASCAL GetFile( HWND hwnd, unsigned msg, WORD wparam,
                            LONG lparam )
{
    WORD        notify;
    HANDLE      handle;
    WORD        index;
    unsigned    total;
    char        str[128];

    switch( msg ) {
    case WM_INITDIALOG:
        if( Ofs->title != NULL ) {
            SetWindowText( hwnd, Ofs->title );
        } else {
            if( Ofs->type == FILE_OPEN ) {
                SetWindowText( hwnd, "OPEN" );
            } else {
                SetWindowText( hwnd, "SAVE" );
            }
        }
        BuildDirectory( hwnd );
        return( TRUE );

    case WM_COMMAND:
        notify = HIWORD( lparam );
        handle = LOWORD( lparam );
        switch( wparam ) {
        case DRIVE_BOX:
            if( notify == LBN_DBLCLK ) {
                index = SendMessage( handle, LB_GETCURSEL, 0 , 0L );
                SendMessage( handle, LB_GETTEXT, index, (LONG) ((LPSTR) str) );
                _dos_setdrive( str[2] -'a'+1,&total );
                BuildDirectory( hwnd );
            }
            break;
        case FILE_BOX:
            if( notify == LBN_DBLCLK ) {
                index = SendMessage( handle, LB_GETCURSEL, 0 , 0L );
                SendMessage( handle, LB_GETTEXT, index, (LONG) ((LPSTR) str) );
                getcwd( Buff, _MAX_PATH );
                if( Buff[strlen(Buff)-1] != '\\' ) strcat( Buff,"\\" );
                strcat( Buff, str );
                if( !CheckSaveVerify() ) break;
                EndDialog( hwnd, TRUE );
            }
            break;
        case DIR_BOX:
            if( notify == LBN_DBLCLK ) {
                index = SendMessage( handle, LB_GETCURSEL, 0 , 0L );
                SendMessage( handle, LB_GETTEXT, index, (LONG) ((LPSTR) str) );
                /*
                 * skip []'s
                 */
                str[ strlen( str )-1 ] = 0;
                chdir( str+1 );
                BuildDirectory( hwnd );
            }
            break;
        case FILE_OK:
            if( !TryTheFile( hwnd ) ) return( FALSE );
            EndDialog( hwnd, TRUE );
            return( TRUE );
            break;
        case FILE_CANCEL:
            EndDialog( hwnd, FALSE );
            return( TRUE );
            break;
        }
    }
    return( FALSE );

} /* GetFile */

/*
 * GetFileName - get a save file name
 */
int __export FAR PASCAL GetFileName( LPFILEOPEN of )
{
    FARPROC     fp;
    BOOL        rc;
    int         i;
    char        *ptr;

    if( of == NULL || Ofs != NULL ) return( 1 );
    fp = MakeProcInstance( (FARPROC)GetFile, Inst );
    Ofs = of;
    strcpy( _ext, Ofs->ext );
    rc = DialogBox( Inst, (LPSTR) "FileOpen", of->hwnd, (DLGPROC)fp );
    FreeProcInstance( fp );
    if( rc ) {
        i = 0;
        ptr = Buff;
        while( 1 ) {
            Ofs->name[i] = *ptr;
            if( *ptr == 0 ) break;
            ptr++;
            i++;
            if( i == Ofs->namelen ) {
                Ofs->name[i-1] = 0;
                break;
            }
        }
    }
    Ofs = NULL;
    return( rc );

} /* GetFileName */
