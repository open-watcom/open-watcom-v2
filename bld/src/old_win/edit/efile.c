#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <string.h>
#include "edit.h"
#include "filedll.h"

static char _ext[] = "*.*";
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
static BOOL GetFileName( LPEDATA ed, int type, char *fname )
{
    FILEOPEN            of;
    HANDLE              dll;
#if defined(__WINDOWS_386__)
    HINDIR              h;
#else
    BOOL                (PASCAL FAR *gsn)( LPFILEOPEN );
#endif
    FARPROC             farp;
    DWORD               a1,a2;
    BOOL                rc;

    dll = LoadLibrary( "filedll.dll" );
    if( dll < 32 ) {
        MessageBox( NULL, "Could not find filedll.dll!", EditTitle, MB_OK );
        return( FALSE );
    }
    farp = (FARPROC) GetProcAddress( dll,"GetFileName" );
#if defined(__WINDOWS_386__)
    h = GetIndirectFunctionHandle( farp, INDIR_PTR, INDIR_ENDLIST );
#else
    gsn = (void FAR *) farp;
#endif

    fname[ 0 ] = 0;
    of.hwnd = ed->hwnd;
    a1 = AllocAlias16( _ext );
    of.ext = (LPSTR) a1;
    a2 = AllocAlias16( fname );
    of.name = (LPSTR) a2;
    of.namelen = _MAX_PATH;
    of.title = (LPSTR) NULL;
    of.type = type;
#if defined(__WINDOWS_386__)
    rc = InvokeIndirectFunction( h, &of );
#else
    rc = gsn( &of );
#endif
    FreeAlias16( a1 );
    FreeAlias16( a2 );
    FreeLibrary( dll );
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
    OFSTRUCT    ofs;
    char        str[128];
    LOCALHANDLE hbuff;
    char        FAR *buffptr;
    int         len;
    int         rc;
    unsigned    bytes;

    if( !ed->needs_saving && !saveas ) return( TRUE );

    /*
     * get filename to save
     */
    if( ed->filename == NULL || saveas ) {
        if( !GetFileName( ed, FILE_SAVE, fname ) ) return( FALSE );
        NewFileName( ed, fname );
    }

    /*
     * try to open the file
     */
    h = OpenFile( ed->filename, &ofs, OF_PROMPT | OF_CANCEL | OF_CREATE );
    if( h < 0 ) {
        sprintf(str, "Cannot save  %s", ed->filename );
        MessageBox( ed->hwnd, str, NULL, MB_OK );
        return( FALSE );
    }

    /*
     * get buffer, and make a copy into near memory, so that
     * we can write it
     */
    hbuff = SendMessage( ed->editwnd, EM_GETHANDLE, 0, 0L );
    buffptr = (char FAR *) MK_LOCAL32( LocalLock( hbuff ) );
    len = _fstrlen( buffptr );
    tmp = MemAlloc( len+1 );
    _fmemcpy( tmp, buffptr, len );
    LocalUnlock( hbuff );

    /*
     * save file, and check results
     */
    rc = _dos_write( h, tmp, len, &bytes );
    MemFree( tmp );
    close( h );

    if( rc || bytes != len ) {
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
        if( !GetFileName( ed, FILE_OPEN, fname ) ) return;
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
    if( len >= 0xFFFFL || (hbuff_new = LocalAlloc( LMEM_MOVEABLE |
                LMEM_ZEROINIT, (WORD) len+1 ) ) == NULL ) {
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
        _fmemcpy( MK_LOCAL32( LocalLock( hbuff_new ) ), buff, len );
        LocalUnlock( hbuff_new );
        MemFree( buff );
    }

    /*
     * clear out old buffer, and set new one
     */
    hbuff_old = SendMessage( ed->editwnd, EM_GETHANDLE, 0, 0L );
    LocalFree( hbuff_old );
    SendMessage( ed->editwnd, EM_SETHANDLE, hbuff_new, 0L );
    NewFileName( ed, fname );
    ed->needs_saving = FALSE;

} /* FileEdit */
