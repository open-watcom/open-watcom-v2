#define INCLUDE_DRIVINIT_H
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "edit.h"

#define PSTR_LEN        256
static char     ProfileString[PSTR_LEN];
static char     _FAR *Title;
static char     *Rest;
static char     *DeviceName;
static char     *DriverName;
static char     *PortName;
static char     _FAR *PrinterData;
static char     DriverFormat[] = "%s.DRV";

/*
 * PrinterDC - obtains a DC to the current printer
 */
HDC PrinterDC( void )
{
    char        *tmp;
    HDC         hdc;
    char        str[128];
    HANDLE      hlib;
    FARPROC     fp;

    /*
     * get printer device type
     */
    PrinterSupport = PSUPP_NONE;
    GetProfileString( "WiNdOwS", "dEvIcE", "", ProfileString, PSTR_LEN );

    /*
     * find first ',' and insert a 0; this is the device name
     */
    DeviceName = ProfileString;
    tmp = ProfileString;
    while( 1 ) {
        if( *tmp == ',' ) {
            *tmp = 0;
            tmp++;
            break;
        }
        if( !(*tmp) ) {
            DeviceName[0] = 0;
            return( NULL );
        }
        tmp++;
    }

    /*
     * find next ',' and insert a 0; this terminates the driver name
     */
    DriverName = tmp;
    while( 1 ) {
        if( *tmp == ',' ) {
            *tmp = 0;
            tmp++;
            break;
        }
        if( !(*tmp) ) {
            DeviceName[0] = 0;
            return( NULL );
        }
        tmp++;
    }
    if( !(*tmp) ) {
        DeviceName[0] = 0;
        return( NULL );
    }
    PortName = tmp;

    /*
     * check if printer has changed, if so, toss the data
     */
    if( PrinterData != NULL && _strcmp( PrinterData, DeviceName ) ) {
        MemFree( PrinterData );
        PrinterData = NULL;
    }

    /*
     * get the printer dc
     */
    hdc = CreateDC( DriverName, DeviceName, PortName, NULL );
    if( hdc == NULL ) return( NULL );
    PrinterSupport = PSUPP_CANPRINT;

    /*
     * look for ExtDeviceMode
     */
    sprintf( str, DriverFormat, DriverName );
    hlib = LoadLibrary( str );
    if( hlib >= 32 ) {
        fp = GetProcAddress( hlib, "EXTDEVICEMODE" );
        if( fp != NULL ) PrinterSupport = PSUPP_CANPRINTANDSET;
        FreeLibrary( hlib );
    }
    return( hdc );

} /* PrinterDC */

#ifndef __WINDOWS_386__
typedef int (FAR PASCAL *EDMPROC)(HWND,HANDLE,LPDEVMODE,LPSTR, LPSTR,LPDEVMODE,LPSTR,WORD);
#endif
/*
 * GetPrinterSetup - get data about this printer
 */
void GetPrinterSetup( HWND hwnd )
{
    char        str[128];
    HANDLE      hlib;
    int         edm_flag;
    char        _FAR *newdata;
    char        _FAR *olddata;
    WORD        bytes;
    WORD        rc;
#ifdef __WINDOWS_386__
    FARPROC     fp;
    HINDIR      hindir;
#else
    EDMPROC     fp;
#endif

    /*
     * get ExtDeviceMode address
     */
    sprintf( str, DriverFormat, DriverName );
    hlib = LoadLibrary( str );
    if( hlib < 32 ) return;
    fp = GetProcAddress( hlib, "EXTDEVICEMODE" );
    if( fp == NULL ) {
        FreeLibrary( hlib );
        return;
    }

    /*
     * get number of bytes
     */
#ifdef __WINDOWS_386__
    hindir = GetIndirectFunctionHandle( fp,
                    INDIR_WORD, INDIR_WORD, INDIR_DWORD,
                    INDIR_PTR, INDIR_PTR, INDIR_DWORD,
                    INDIR_DWORD, INDIR_WORD, INDIR_ENDLIST );
    bytes = (WORD) InvokeIndirectFunction( hindir, hwnd, hlib, NULL,
                        DeviceName, PortName, NULL, NULL, 0 );
    MemFree( (void *) hindir );
#else
    bytes = fp( hwnd, hlib, (LPDEVMODE) NULL, (LPSTR) DeviceName,
                  (LPSTR) PortName, (LPDEVMODE) NULL, (LPSTR) NULL, 0);
#endif
    /*
     * if we already have data, use that in the dialog
     */
    newdata = MemAlloc( bytes );
    edm_flag = DM_PROMPT | DM_COPY;
    if( PrinterData != NULL ) edm_flag |= DM_MODIFY;
    olddata = PrinterData;

    /*
     * now, get the new data from the dialog
     */
#ifdef __WINDOWS_386__
    hindir = GetIndirectFunctionHandle( fp,
                        INDIR_WORD, INDIR_WORD, INDIR_PTR,
                        INDIR_PTR, INDIR_PTR, INDIR_PTR,
                        INDIR_DWORD, INDIR_WORD, INDIR_ENDLIST );
    rc = InvokeIndirectFunction(
                        hindir,
                        hwnd,
                        hlib,
                        (LPDEVMODE) newdata,
                        (LPSTR) DeviceName,
                        (LPSTR) PortName,
                        (LPDEVMODE) olddata,
                        0L,
                        edm_flag );
    MemFree( (void *) hindir );
#else
    rc =  fp( hwnd, hlib, (LPDEVMODE) newdata, (LPSTR) DeviceName,
                 (LPSTR) PortName, (LPDEVMODE) olddata,
                 (LPSTR) NULL, edm_flag );
#endif

    FreeLibrary( hlib );
    if( rc != IDOK ) {
        MemFree( newdata );
    } else {
        MemFree( olddata );
        PrinterData = newdata;
    }

} /* GetPrinterSetup */

static volatile BOOL WasAborted;
static HWND DlgWnd;

/*
 * Abort - procedure to abort a print job
 */
int _EXPORT FAR PASCAL Abort( HDC hdc, WORD w )
{
    MSG msg;

    w = w;      /* shut up warning */
    hdc = hdc;

    while( !WasAborted && PeekMessage( &msg, NULL, NULL, NULL, TRUE )) {
        if( !IsDialogMessage( DlgWnd, &msg ) ) {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }
    }
    return( !WasAborted );

} /* Abort */

/*
 * AbortDialog - wait for the person to press cancel
 */
int _EXPORT FAR PASCAL AbortDialog( HWND hwnd, unsigned msg, WORD wparam,
                                LONG lparam )
{
    lparam = lparam;
    wparam = wparam;

    switch(msg) {
    case WM_COMMAND:
        WasAborted = TRUE;
        return( TRUE );

    case WM_INITDIALOG:
        SetDlgItemText( hwnd, PRINT_TITLE, (LPSTR) Title );
        SetDlgItemText( hwnd, PRINT_REST, (LPSTR) Rest );
        return( TRUE );
    }
    return( FALSE );

} /* AbortDialog */


/*
 * CleanUp - clean up after print
 */
static BOOL CleanUp( HDC hdc, FARPROC abort, FARPROC abortdlg, BOOL err )
{

    if( err ) Escape( hdc, ABORTDOC, 0, NULL, NULL );
    if( DlgWnd != NULL ) DestroyWindow( DlgWnd );
    DlgWnd = NULL;
    if( hdc != NULL ) DeleteDC( hdc );
    if( abortdlg != NULL ) FreeProcInstance( abortdlg );
    if( abort != NULL ) FreeProcInstance( abort );
    /* Error? make sure the user knows... */
    if( WasAborted ) {
        MessageBox( NULL, Title, "Error printing file", MB_OK );
    }
    return( 0 );

} /* CleanUp */

/*
 * Print - print current file
 */
BOOL Print( LPEDATA ed )
{
    FARPROC     abort;
    FARPROC     abortdlg;
    HDC         hdc;
    WORD        pageheight;
    WORD        lineheight;
    WORD        height;
    WORD        linecnt;
    WORD        currline;
    WORD        offset;
    WORD        len;
    LOCALHANDLE hdata;
    BOOL        error;
    char        FAR *buff;
    char        _FAR *tmpbuff;
    char        rest[256];

    /*
     * init. misc.
     */
    Title = ed->filename;
    if( Title == NULL ) Title = (LPSTR) "untitled";
    WasAborted = FALSE;
    error = FALSE;
    Rest = rest;
    sprintf( Rest,"%s %s", PortName, DeviceName );

    /*
     * create procs
     */
    abort = MakeProcInstance( Abort, ed->inst );
    if( abort == NULL ) return( 0 );
    abortdlg = MakeProcInstance( AbortDialog, ed->inst );
    if( abortdlg == NULL ) return( CleanUp( NULL, abort, NULL, FALSE ) );

    /*
     * init. printer
     */
    hdc = PrinterDC();
    if( !hdc ) return( CleanUp( NULL, abort, abortdlg, FALSE ) );

    /*
     * Create abort dialog
     */
    DlgWnd = CreateDialog( ed->inst, "AbortDialog", ed->hwnd, abortdlg );
    if( DlgWnd == NULL ) return( CleanUp( hdc, abort, abortdlg, 0 ) );
    ShowWindow( DlgWnd, SW_SHOW );
    UpdateWindow( DlgWnd );

    /*
     * set up escape function
     */
    if( Escape( hdc, SETABORTPROC, 0, (LPSTR) abort, NULL ) < 0 ) {
        return( CleanUp( hdc, abort, abortdlg, FALSE ) );
    }

    /*
     * start document
     */
    if( Escape( hdc, STARTDOC, _strlen( Title ), (LPSTR) Title, NULL ) < 0 ) {
        return( CleanUp( hdc, abort, abortdlg, FALSE ) );
    }

    /*
     * get info
     */
    pageheight = GetDeviceCaps( hdc, VERTRES );
    lineheight = HIWORD( GetTextExtent( hdc, "CC", 2 ) );
    height = 0;
    linecnt = SendMessage( ed->editwnd, EM_GETLINECOUNT, 0, 0L );
    currline = 0;
    hdata = SendMessage( ed->editwnd, EM_GETHANDLE, 0, 0L );

    /*
     * print all lines
     */
    while( currline < linecnt ) {

        if( height + lineheight > pageheight ) {
            height = 0;
            if( Escape( hdc, NEWFRAME, 0, NULL, NULL ) < 0 ) error = TRUE;
        }
        if( WasAborted || error ) return( CleanUp( hdc, abort, abortdlg, TRUE ) );

        /*
         * get the data and print it
         */
        offset = SendMessage( ed->editwnd, EM_LINEINDEX, currline, 0L );
        len = SendMessage( ed->editwnd, EM_LINELENGTH, offset, 0L );
        buff = MK_LOCAL32( LocalLock( hdata ) ) ;
        tmpbuff = MemAlloc( len );
        _fmemcpy( tmpbuff, buff + offset, len );
        LocalUnlock( hdata );

        TextOut( hdc, 0, height, (LPSTR)tmpbuff, len );
        MemFree( tmpbuff );

        currline++;
        height += lineheight;
    }

    /*
     * eject final page, and tidy up
     */
    if( Escape( hdc, NEWFRAME, 0, NULL, NULL) >= 0 ) {
        if( Escape( hdc, ENDDOC, 0, NULL, NULL ) < 0 ) error = TRUE;
    } else {
        error = TRUE;
    }
    return( CleanUp( hdc, abort, abortdlg, error ) );

} /* Print */
