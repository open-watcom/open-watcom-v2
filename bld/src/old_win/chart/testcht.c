#include <stdio.h>
#include <windows.h>
#include <stddef.h>
#include "chart.h"
#include "testcht.h"
#include "ctltype.h"    // for bar dialog

/* data structure for the bar dialog. Uses the Data Control example
   to define and process the dialog */
typedef struct {
    float               value;
    int                 red;
    int                 green;
    int                 blue;
} dlg_data;

static dlg_data BarData;

#include "dialog.h"
#include "bardlg.h"

static float Values[]={
    13.0,
    1.1,
    8.6,
    5.0,
    18.0
};

#ifdef __WINDOWS_386__
/*
 * used to invoke DLL functions
 */
HINDIR ChartAddHandle;
HINDIR ChartDrawHandle;
HINDIR ChartBarClickHandle;
HINDIR ChartSetBarColorHandle;
HINDIR ChartGetBarColorHandle;
HINDIR ChartDeleteHandle;
HINDIR ChartCloseUseHandle;
HINDIR ChartInitUseHandle;
#endif

static char TestchtClass[32]="TestchtClass";

void CreateMe( HWND );
static BOOL FirstInstance( HINSTANCE );
static BOOL AnyInstance( HINSTANCE, int, LPSTR );
long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, WORD, LONG );


/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline,
                    int cmdshow )
{
    MSG         msg;

#ifdef __WINDOWS_386__
    sprintf( TestchtClass,"TestchtClass%d", this_inst );
    if( !LoadChartDLL() ) return( FALSE) ;
    prev_inst = prev_inst;                      /* stop warning messsage */
#endif

    ChartInitUse();

#ifndef __WINDOWS_386__
    if( !prev_inst )
#endif
        if( !FirstInstance( this_inst ) ) return( FALSE );

    if( !AnyInstance( this_inst, cmdshow, cmdline ) ) return( FALSE );

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

    ChartCloseUse();

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HINSTANCE this_inst )
{
    WNDCLASS    wc;
    BOOL        rc;


    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( WORD );
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, "TestchtIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "TestchtMenu";
    wc.lpszClassName = TestchtClass;
    rc = RegisterClass( &wc );
    return( rc );

} /* FirstInstance */

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HINSTANCE this_inst, int cmdshow, LPSTR cmdline )
{
    HWND        wind_handle;

    cmdline = cmdline;

    /*
     * create main window
     */
    wind_handle = CreateWindow(
        TestchtClass,           /* class */
        "Testcht: double click bars to change color/data",
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !wind_handle ) return( FALSE );

    /*
     * display window
     */
    ShowWindow( wind_handle, cmdshow );
    UpdateWindow( wind_handle );

    return( TRUE );

} /* AnyInstance */

/*
 * AboutDlgProc - processes messages for the about dialogue.
 */
BOOL _EXPORT FAR PASCAL About( HWND window_handle, unsigned msg,
                                WORD wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */
    window_handle = window_handle;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK ) {
            EndDialog( window_handle, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutDlgProc */

/*
 * FetchData - Fetch data for the chart
 */
BOOL _EXPORT FAR PASCAL FetchData( int which, LPFLOAT value )

{
float far *v;

    if( which < 5 ) {
        v = MK_FP32( value );
        *v = Values[which];
    } else {
        return( FALSE );
    }

    return( TRUE );
}

/*
 * BarDlg - processes messages for tr info dialog.
 */
BOOL _EXPORT FAR PASCAL BarDlg( HWND window_handle, unsigned msg,
                                WORD wparam, LONG lparam )
{
    switch( msg ) {
    case WM_INITDIALOG:
        /* get the controls in the dialog initialized with the correct
           settings based on the control data */
        ctl_dlg_init( GetWindowWord( window_handle, GWW_HINSTANCE ),
                                        window_handle, &BarData, &DlgData );
        return( TRUE );         // let WINDOWS set the input focus

    case WM_COMMAND:
        switch( wparam ) {
        case IDOK:
            if( ctl_dlg_done( GetWindowWord( window_handle, GWW_HINSTANCE ),
                                    window_handle, &BarData, &DlgData ) ) {
                EndDialog( window_handle, TRUE );
            }
            return( TRUE );

        case IDCANCEL:
            EndDialog( window_handle, FALSE );
            return( TRUE );
        }
    }

    /* let the data control library look at the message to see if any
       controls have been modified */
    ctl_dlg_process( &DlgData, wparam, lparam );

    return( FALSE );
} /* BarDlg */

/*
 * WindowProc - handle messages for the main application window
 */
LONG _EXPORT FAR PASCAL WindowProc( HWND window_handle, unsigned msg,
                                     WORD wparam, LONG lparam )
{
    FARPROC             proc;
    HANDLE              inst_handle;
    PAINTSTRUCT         paint;
    HDC                 win_dc;
    WORD                bar;
    COLORREF            color;
    chart_hld           chart;
    int                 dlg_ret;

    switch( msg ) {

    case WM_CREATE:
        CreateMe( window_handle );      /* in testcht2.h */
        break;

    case WM_COMMAND:
        switch( wparam ) {

        case MENU_ABOUT:
            inst_handle = GetWindowWord( window_handle, GWW_HINSTANCE );
            proc = MakeProcInstance( (FARPROC)About, inst_handle );
            DialogBox( inst_handle,"AboutBox", window_handle, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;
        }
        break;

    case WM_PAINT:
        win_dc = BeginPaint( window_handle, &paint );
        ChartDraw( GetWindowWord( window_handle, 0 ), window_handle, win_dc );
        EndPaint( window_handle, &paint );
        break;

    case WM_LBUTTONDBLCLK:
        chart = GetWindowWord( window_handle, 0 );
        bar = ChartBarClick( LOWORD( lparam ), HIWORD( lparam), chart );
        if( bar != -1 ) {
            BarData.value = Values[bar];

            color = ChartGetBarColor( bar, chart );
            BarData.red = GetRValue( color );
            BarData.green = GetGValue( color );
            BarData.blue = GetBValue( color );
            inst_handle = GetWindowWord( window_handle, GWW_HINSTANCE );
            proc = MakeProcInstance( (FARPROC)BarDlg, inst_handle );
            dlg_ret = DialogBox( inst_handle,"BARDLG", window_handle, (DLGPROC)proc );
            FreeProcInstance( proc );
            if( dlg_ret != -1 && dlg_ret ) {
                Values[bar] = BarData.value;
                ChartSetBarColor( bar, RGB( BarData.red, BarData.green,
                                                    BarData.blue ), chart );
                InvalidateRect( window_handle, NULL, TRUE );
            }
        }
        break;

    case WM_SIZE:
        InvalidateRect( window_handle, NULL, TRUE );
        break;

    case WM_DESTROY:
        ChartDelete( GetWindowWord( window_handle, 0 ) );
        PostQuitMessage( 0 );
        break;

    default:
        return( DefWindowProc( window_handle, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */

#ifdef __WINDOWS_386__
/*
 * LoadChartDLL - get DLL ready for 32-bit use
 */
static int LoadChartDLL( void )
{
HANDLE  dll;
FARPROC addr;

    dll = LoadLibrary( "chart.dll" );
    if( dll < 32 ) return( FALSE);

    addr = GetProcAddress( dll,"ChartAdd" );
    ChartAddHandle = GetIndirectFunctionHandle( addr,
                        INDIR_DWORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartDraw" );
    ChartDrawHandle = GetIndirectFunctionHandle( addr,
                        INDIR_WORD, INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartBarClick" );
    ChartBarClickHandle = GetIndirectFunctionHandle( addr,
                        INDIR_WORD, INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartSetBarColor" );
    ChartSetBarColorHandle = GetIndirectFunctionHandle( addr,
                        INDIR_WORD, INDIR_DWORD, INDIR_WORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartGetBarColor" );
    ChartGetBarColorHandle = GetIndirectFunctionHandle( addr,
                        INDIR_WORD, INDIR_WORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartDelete" );
    ChartDeleteHandle = GetIndirectFunctionHandle( addr,
                        INDIR_WORD, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartCloseUse" );
    ChartCloseUseHandle = GetIndirectFunctionHandle( addr, INDIR_ENDLIST );

    addr = GetProcAddress( dll,"ChartInitUse" );
    ChartInitUseHandle = GetIndirectFunctionHandle( addr, INDIR_ENDLIST );
    return( TRUE );
}
#endif
