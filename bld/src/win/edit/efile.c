#define INCLUDE_COMMDLG_H       1
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <string.h>
#include "edit.h"
#include "win1632.h"

static char _savemsg[] = "Save current changes";
#ifdef __WINDOWS_386__
static char _fmt[] = "%s: %s";
#else
static char _fmt[] = "%s: %Fs";
#endif

/*
 * CheckFileSave - check if file needs to be saved; if so,
 *                 ask the user if it should be or not
 */
BOOL CheckFileSave( LPEDATA ed )
{
char    str[128];
int     rc;

    if( ed->needs_saving ) {
        if( ed->filename == NULL ) {
            strcpy( str, _savemsg );
        } else {
            sprintf( str, _fmt, _savemsg, ed->filename );
        }
        rc = MessageBox( ed->hwnd, str, EditTitle,  MB_YESNOCANCEL );
        if( rc == IDYES ) {
            if( FileSave( ed, FALSE ) ) return( TRUE );
        } else if( rc == IDNO ) {
            return( TRUE );
        }
        return( FALSE );
    }
    return( TRUE );

} /* CheckFileSave */

/*
 * GetFileName - get a file name using common dialog stuff
 */
static BOOL GetFileName( LPEDATA ed, BOOL save, char *fname )
{
    static char         filterList[] = "File (*.*)" \
                                        "\0" \
                                        "*.*" \
                                        "\0\0";
    OPENFILENAME        of;
    int                 rc;

    fname[ 0 ] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = ed->hwnd;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = "";
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.Flags = OFN_HIDEREADONLY;
    if( save ) {
        rc = GetSaveFileName( &of );
    } else {
        rc = GetOpenFileName( &of );
    }
    return( rc );
} /* GetFileName */

/*
 * NewFileName - set a new file name
 */
static void NewFileName( LPEDATA ed, char *fname )
{
char    str[256];
int     len;

        MemFree( ed->filename );
        len = strlen( fname );
        if( len != 0 ) {
            ed->filename = MemAlloc( len + 1 );
            _memcpy( ed->filename, fname, len + 1 );
            sprintf( str,"%s - %s", EditTitle, fname );
        } else {
            ed->filename = NULL;
            strcpy( str, EditTitle );
        }
        SetWindowText( ed->hwnd, str );

} /* NewFileName */


/*
 * FileSave - save a file
 */
BOOL FileSave( LPEDATA ed, BOOL saveas )
{
    char        fname[_MAX_PATH];
    char        _FAR *tmp;
    int         h;
    char        str[128];
    LOCALHANDLE hbuff;
    char        FAR *buffptr;
    int         len;
    unsigned    written;
    int         rc;

    if( !ed->needs_saving && !saveas ) return( TRUE );

    /*
     * get filename to save
     */
    if( ed->filename == NULL || saveas ) {
    if( !GetFileName( ed, TRUE, fname ) ) return( FALSE );
        NewFileName( ed, fname );
    }

    /*
     * try to open the file
     */
    h = open( fname,  O_CREAT | O_TRUNC | O_WRONLY | O_TEXT,
                S_IRWXU | S_IRWXG | S_IRWXO );
    if( h < 0 ) {
        sprintf(str, "Cannot save  %s", ed->filename );
        MessageBox( ed->hwnd, str, NULL, MB_OK );
        return( FALSE );
    }

    /*
     * get buffer, and make a copy into near memory, so that
     * we can write it
     */
#ifdef __NT__
// there seems to be a bug in the win32s support for EM_GETHANDLE
// so avoid using it in the win32 version
    tmp = tmp;
    hbuff = hbuff;
    buffptr = buffptr;
    len = GetWindowTextLength( ed->editwnd );
    tmp = MemAlloc( len + 1 );
    GetWindowText( ed->editwnd, tmp, len + 1 );
#else
    hbuff = (HANDLE)SendMessage( ed->editwnd, EM_GETHANDLE, 0, 0L );
    buffptr = (char FAR *) MK_LOCAL32( LocalLock( hbuff ) );
    len = _fstrlen( buffptr );
    tmp = MemAlloc( len+1 );
    _fmemcpy( tmp, buffptr, len );
    LocalUnlock( hbuff );
#endif


    /*
     * save file, and check results
     */
    rc = _dos_write( h, tmp, len, &written );
    MemFree( tmp );
    close( h );

    if( len != written ) {
        sprintf( str, "Error writing to %s", ed->filename );
        MessageBox( ed->hwnd, str, EditTitle, MB_OK );
        return( FALSE );
    } else {
        ed->needs_saving = FALSE;
        return( TRUE );
    }

} /* FileSave */

/*
 * FileEdit - edit a new file
 */
void FileEdit( LPEDATA ed, BOOL openfile )
{
    LOCALHANDLE hbuff_new,hbuff_old;
    char        fname[_MAX_PATH];
    char        str[128];
    long int    len=0;
    char        _FAR *buff;
    int         rc;
    int         h;
    unsigned    bytes;

    if( !CheckFileSave( ed ) ) return;
    if( openfile ) {
        if( !GetFileName( ed, FALSE, fname ) ) return;
    } else {
        fname[0] = 0;
    }

    /*
     * get file size, and get a buffer
     */
    if( fname[0] != 0 ) {
        h = open( fname, O_RDONLY | O_BINARY );
        if( h < 0 ) {
            sprintf( str,"Could not open file %s", fname );
            MessageBox( ed->hwnd, str, EditTitle, MB_OK );
            return;
        }
        len = filelength( h );
    }
    hbuff_new = NULL;
    if( len < 0xFFFFL ) {
        hbuff_new = LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT, len + 1 );
    }
    if( hbuff_new == NULL ) {
        sprintf( str, "File %s is too big (%ld bytes)", fname, len+1 );
        MessageBox( ed->hwnd, str, EditTitle, MB_OK );
        return;
    }

    /*
     * read into temp. buffer, then copy to edit area
     */
    if( fname[0] != 0 ) {
        buff = MemAlloc( len + 1 );
        rc = _dos_read( h, buff, len, &bytes );
        close( h );
        if( rc || bytes != len ) {
            sprintf( str,"Could not read file %s", fname );
            MessageBox( ed->hwnd, str, EditTitle, MB_OK );
            LocalFree( hbuff_new );
            MemFree( buff );
            return;
        }
    #ifdef __NT__
        memcpy( MK_LOCAL32( LocalLock( hbuff_new ) ), buff, (size_t)len );
    #else
        _fmemcpy( MK_LOCAL32( LocalLock( hbuff_new ) ), buff, (size_t)len );
    #endif
        LocalUnlock( hbuff_new );
        MemFree( buff );
    }

    /*
     * clear out old buffer, and set new one
     */
#ifdef __NT__
    // there seems to be a bug in the Win32s support for the EM_SETHANDLE
    // message
    hbuff_old = hbuff_old;
    SetWindowText( ed->editwnd, LocalLock( hbuff_new ) );
    LocalUnlock( hbuff_new );
    LocalFree( hbuff_new );
#else
    hbuff_old = (HANDLE)SendMessage( ed->editwnd, EM_GETHANDLE, 0, 0L );
    SendMessage( ed->editwnd, EM_SETHANDLE, (UINT)hbuff_new, 0L );
    LocalFree( hbuff_old );
#endif
    NewFileName( ed, fname );
    ed->needs_saving = FALSE;

} /* FileEdit */
