#include <string.h>
#include <stdio.h>

#define DEFINE_GLOBAL_VARS
#include "life.h"

static char LifeClass[32]="LifeClass";
static BOOL AnyInstance( HANDLE this_inst, int cmdshow );
static BOOL FirstInstance( HANDLE this_inst );

extern void Error( char *str )
/*****************************
    Pop up an error message box
*/
{
    MessageBox( NULL, str, Buffer, MB_ICONHAND+MB_OK+MB_SYSTEMMODAL );
}


extern BOOL NoMemory()
/********************/
{
    Error( "Out of memory" );
    return( FALSE );
}


int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst,
                    LPSTR cmdline, int cmdshow )
/***********************************************

    Initialization, message loop.
*/
{
    MSG         msg;

    cmdline = cmdline;

#ifdef __WINDOWS_386__
    sprintf( LifeClass,"LifeClass%d", this_inst );
    prev_inst = 0;
#endif
    if( !prev_inst ) {
        if( !FirstInstance( this_inst ) ) return( FALSE );
    }
    if( !AnyInstance( this_inst, cmdshow ) ) return( FALSE );

    while( GetMessage( &msg, NULL, NULL, NULL ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }

    return( msg.wParam );

}

extern long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

static BOOL FirstInstance( HANDLE this_inst )
/********************************************

    Register window class for the application,
    and do any other application initialization.
*/
{
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_HREDRAW+CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = this_inst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "LifeMenu";
    wc.lpszClassName = LifeClass;
    rc = RegisterClass( &wc );
    return( rc );

}

static BOOL AnyInstance( HANDLE this_inst, int cmdshow )
/*******************************************************

    Do work required for every instance of the application:
    create the window, initialize data.
*/
{
    pixels      screen_x, screen_y;

    screen_x = GetSystemMetrics( SM_CXSCREEN );
    screen_y = GetSystemMetrics( SM_CYSCREEN );
    ScreenHeight = screen_y;

    ThisInst = this_inst;

    if( !ReadPatterns() ) return( FALSE );

    Pen = (HPEN)GetStockObject( BLACK_PEN );
    Brush = (HBRUSH)GetStockObject( HOLLOW_BRUSH );

    WinHandle = CreateWindow(
        LifeClass,              /* class */
        "Life",                 /* caption */
        WS_OVERLAPPEDWINDOW,    /* style */
        screen_x / 8,           /* init. x pos */
        screen_y / 8,           /* init. y pos */
        3 * screen_x / 4,       /* init. x size */
        3 * screen_y / 4,       /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !WinHandle ) return( FALSE );

    /*
     * display window
     */
    ShowWindow( WinHandle, cmdshow );
    UpdateWindow( WinHandle );

    Births[3] = TRUE;
    memset( Deaths, TRUE, sizeof( Deaths ) );
    Deaths[2] = FALSE;
    Deaths[3] = FALSE;
    CurvedSpace = TRUE;
    Mode = MENU_RESUME;
    MouseMode = MENU_FLIP_PATTERNS;
    return( InitTimer() );
}

BOOL _EXPORT FAR PASCAL About( HWND win_handle, unsigned msg,
                               UINT wparam, LONG lparam )
/************************************************************

    Process messages for the rules dialogue.
*/
{
    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD(wparam) == IDOK ) {
            EndDialog( win_handle, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

}


BOOL _EXPORT FAR PASCAL Rules( HWND win_handle, unsigned msg,
                                UINT wparam, LONG lparam )
/************************************************************

    Process messages for the rules dialogue.
*/
{
    static BOOL         WorkBirths[9], WorkDeaths[9];

    int         i;

    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        for( i = 0; i < 9; ++i ) {
            WorkBirths[ i ] = Births[ i ];
            WorkDeaths[ i ] = Deaths[ i ];
            CheckDlgButton( win_handle, BM_BIRTH_0+i, WorkBirths[ i ] );
            CheckDlgButton( win_handle, BM_DEATH_0+i, WorkDeaths[ i ] );
        }
        return( TRUE );

    case WM_COMMAND:
        switch( LOWORD(wparam) ) {
        case IDOK:
            for( i = 0; i < 9; ++i ) {
                Births[ i ] = WorkBirths[ i ];
                Deaths[ i ] = WorkDeaths[ i ];
            }
            /* fall through to next case */
        case IDCANCEL:
            EndDialog( win_handle, TRUE );
            return( TRUE );
        case BM_BIRTH_0:
        case BM_BIRTH_1:
        case BM_BIRTH_2:
        case BM_BIRTH_3:
        case BM_BIRTH_4:
        case BM_BIRTH_5:
        case BM_BIRTH_6:
        case BM_BIRTH_7:
        case BM_BIRTH_8:
            i = LOWORD(wparam) - BM_BIRTH_0;
            WorkBirths[ i ] = !WorkBirths[ i ];
            CheckDlgButton( win_handle, wparam, WorkBirths[ i ] );
            return( TRUE );
        case BM_DEATH_0:
        case BM_DEATH_1:
        case BM_DEATH_2:
        case BM_DEATH_3:
        case BM_DEATH_4:
        case BM_DEATH_5:
        case BM_DEATH_6:
        case BM_DEATH_7:
        case BM_DEATH_8:
            i = LOWORD(wparam) - BM_DEATH_0;
            WorkDeaths[ i ] = !WorkDeaths[ i ];
            CheckDlgButton( win_handle, wparam, WorkDeaths[ i ] );
            return( TRUE );
        case BM_DEATH_NEVER:
            for( i = 0; i < 9; ++i ) {
                WorkDeaths[ i ] = FALSE;
                CheckDlgButton( win_handle, BM_DEATH_0+i, WorkDeaths[ i ] );
            }
            return( TRUE );
        }
    }
    return( FALSE );

}



extern void FlushMouse()
/***********************

    Flush out any pending mouse events.
*/
{
    MSG         peek;
    while( PeekMessage( &peek, WinHandle, WM_MOUSEFIRST,
                        WM_MOUSELAST, PM_REMOVE ) );
}


static void DisplayDialog( char *name, BOOL _EXPORT FAR PASCAL rtn() )
/**********************************************************************

    Display a given dialog box.
*/
{
    FARPROC     proc;

    proc = MakeProcInstance( rtn, ThisInst );
    DialogBox( ThisInst, name, WinHandle, proc );
    FreeProcInstance( proc );
}


static void ToPauseMode()
{
    Mode = MENU_PAUSE;
}

static void ToResumeMode()
{
    Mode = MENU_RESUME;
    SelectOff();
    MouseMode = MENU_FLIP_PATTERNS;
}


static void ToSelectMode()
{
    MouseMode = MENU_SELECT;
    Mode = MENU_PAUSE;
}

static void ToPatternFlipMode()
{
    SelectOff();
    MouseMode = MENU_FLIP_PATTERNS;
}


static void ToSingleStepMode()
{
    SelectOff();
    MouseMode = Mode = MENU_SINGLE_STEP;
}


static void MenuItem( WORD wparam )
/**********************************

    Handle a menu item which has been selected by the user.
*/
{
    if( wparam >= MENU_PATTERN && wparam <= MENU_PATTERN+NumberPatterns ) {
        SelectOff();
        MouseMode = MENU_FLIP_PATTERNS;
        SetCurrPattern( wparam-MENU_PATTERN );
        return;
    }
    switch( wparam ) {
    case MENU_ABOUT:
        DisplayDialog( "AboutBox", About );
        break;
    case MENU_WRAP_AROUND:
        CurvedSpace = TRUE;
        break;
    case MENU_BOUNDED_EDGES:
        CurvedSpace = FALSE;
        break;
    case MENU_SINGLE_STEP:
        ToSingleStepMode();
        break;
    case MENU_PAUSE:
        ToPauseMode();
        break;
    case MENU_RESUME:
        ToResumeMode();
        break;
    case MENU_FLIP_PATTERNS:
        ToPatternFlipMode();
        break;
    case MENU_SELECT:
        ToSelectMode();
        break;
    case MENU_SAVE:
        WritePatternFile();
        break;
    case MENU_LOAD:
        LoadNewPattern();
        break;
    case MENU_FASTEST:
        TimerTurbo();
        ToResumeMode();
        break;
    case MENU_CLEAR:
        Clear();
        if( !SelectOn() ) ToPauseMode();
        break;
    case MENU_GRID:
        DrawGrid = !DrawGrid;
        RePaint();
        break;
    case MENU_RANDOM:
        Randomize();
        break;
    case MENU_LOAD_BITMAP:
        LoadNewBitmap();
        SelectOff();
        ReSizeArray( WindowWidth, WindowHeight, 0 );
        RePaint();
        break;
    case MENU_NEW_RULES:
        DisplayDialog( "Rules", Rules );
        break;
    case MENU_ROTATE_M90:
        TransformPatterns( ReflectAboutXequalY );
        TransformPatterns( ReflectAboutYAxis );
        /* fall through */
    case MENU_ROTATE_180:
        TransformPatterns( ReflectAboutXequalY );
        TransformPatterns( ReflectAboutYAxis );
        /* fall through */
    case MENU_ROTATE_P90:
        TransformPatterns( ReflectAboutXequalY );
        TransformPatterns( ReflectAboutYAxis );
        CreatePatternMenu();
        break;
    case MENU_REFLECT_X:
        TransformPatterns( ReflectAboutXequalY );
        TransformPatterns( ReflectAboutYAxis );
        TransformPatterns( ReflectAboutXequalY );
        CreatePatternMenu();
        break;
    case MENU_REFLECT_Y:
        TransformPatterns( ReflectAboutYAxis );
        CreatePatternMenu();
        break;
    default:
        break;
    }
}


static  int Check[] = { MF_UNCHECKED, MF_CHECKED };
static  int Gray[] = { MF_GRAYED, MF_ENABLED };

static void InitMenu( HMENU mh )
/*******************************

    Initialize the menu display. Disable any items which are not applicable.
*/
{
    int         i;

    CheckMenuItem( mh, MENU_WRAP_AROUND, Check[ CurvedSpace ] );
    CheckMenuItem( mh, MENU_BOUNDED_EDGES, Check[ !CurvedSpace ] );
    CheckMenuItem( mh, MENU_PAUSE, Check[ Mode == MENU_PAUSE ] );
    CheckMenuItem( mh, MENU_RESUME, Check[ Mode == MENU_RESUME ] );
    CheckMenuItem( mh, MENU_SINGLE_STEP, Check[ MouseMode == MENU_SINGLE_STEP ]);
    CheckMenuItem( mh, MENU_SELECT, Check[ MouseMode == MENU_SELECT ]);
    CheckMenuItem( mh, MENU_FLIP_PATTERNS, Check[ MouseMode == MENU_FLIP_PATTERNS ]);
    EnableMenuItem( mh, MENU_SAVE, Gray[ SelectOn() ]);
    CheckMenuItem( mh, MENU_GRID, Check[ DrawGrid ]);
    for( i = 0; i < NumberPatterns; ++i ) {
        CheckMenuItem( mh, MENU_PATTERN+i, Check[ IsCurrPattern( i ) ] );
    }
}

static BOOL SingleStep()
/***********************

    Process a single step request
*/
{
    if( Mode != MENU_SINGLE_STEP ) return( FALSE );
    NextGeneration();
    return( TRUE );
}


extern void SetCaption()
/***********************

    Set the caption to indicate generation number, etc.
*/
{
    static BOOL IconCaptionSet = FALSE;

    if( IsAnIcon ) {
        if( !IconCaptionSet ) {
            SetWindowText( WinHandle, "Life" );
            IconCaptionSet = TRUE;
        }
    } else {
        IconCaptionSet = FALSE;
        sprintf( Buffer, "Life: Pop - %ld; Gen - %ld; Max - %ld",
                 Population, Generation, (long)ArraySizeX*ArraySizeY);
        SetWindowText( WinHandle, Buffer );
    }
}


static void Cleanup()
/********************

    Free up all our memory, etc
*/
{
    DeleteObject( Pen );
    DeleteObject( Brush );
    FreeArray( CellArray );
    FreePatterns();
    FiniBitMap();
    FiniTimer();
}


LONG _EXPORT FAR PASCAL WindowProc( HWND win_handle, unsigned msg,
                                     UINT wparam, LONG lparam )
/**************************************************************

    Handle messages for the main application window.
*/
{
    WinHandle = win_handle;
    switch( msg ) {
    case WM_CREATE:
        InitBitMap();
        CreatePatternMenu();
        break;

    case WM_INITMENU:
        InitMenu( (HMENU)wparam );
        break;

    case WM_COMMAND:
        MenuItem( LOWORD( wparam ) );
        break;

    case WM_PAINT:
        RePaint();
        break;

    case WM_TIMER:
        if( Mode != MENU_RESUME ) break;
        NewTimer();
        NextGeneration();
        CheckTimerRate();
        break;

    case WM_SIZE:
        SelectOff();
        if( ReSizeArray( LOWORD( lparam ), HIWORD( lparam ), wparam ) ) break;
        NoMemory();

    case WM_DESTROY:
        Cleanup();
        PostQuitMessage( 0 );
        return( DefWindowProc( WinHandle, msg, wparam, lparam ) );
        break;

    case WM_LBUTTONUP:
        EndSelect( LOWORD( lparam ), HIWORD( lparam ) );
        break;
    case WM_LBUTTONDOWN:
        if( StartSelect( LOWORD( lparam ), HIWORD( lparam ) ) ) break;
    case WM_RBUTTONDOWN:
        if( SingleStep() ) break;
        /* fall through */
    case WM_MOUSEMOVE:
        if( MoveSelect( wparam, LOWORD( lparam ), HIWORD( lparam ) ) ) break;
        FlipPattern( wparam, LOWORD( lparam ), HIWORD( lparam ) );
        break;
    case WM_KEYDOWN:
        SingleStep();
        break;
    default:
        return( DefWindowProc( WinHandle, msg, wparam, lparam ) );
    }
    return( 0L );
}


static void UpdateCell( HDC dc, cell_ptr cell, pixels x, pixels y )
/******************************************************************

    Update the cell at location (x,y)
*/
{
    if( cell->alive && !cell->drawn ) {
        BlitBitMap( dc, x, y );
        cell->drawn = 1;
    } else if( !cell->alive && cell->drawn ) {
        UnBlitBitMap( dc, x, y );
        cell->drawn = 0;
    }
}


extern BOOL TurnOnCell( HDC dc, cell_ptr cell, pixels x, pixels y )
/******************************************************************

    Turn on the "cell" at grid location (x,y) if needed
*/
{
    if( cell->alive ) return( FALSE );
    ++Population;
    cell->alive = 1;
    UpdateCell( dc, cell, x, y );
    return( TRUE );
}


extern BOOL TurnOffCell( HDC dc, cell_ptr cell, pixels x, pixels y )
/******************************************************************

    Turn off the "cell" at grid location (x,y) if needed
*/
{
    cell = CellPointer( x, y );
    if( !cell->alive ) return( FALSE );
    --Population;
    cell->alive = 0;
    UpdateCell( dc, cell, x, y );
    return( TRUE );
}


static void RePaint()
/********************

    Re-draw the entire screen. It's been trashed.
*/
{
    PAINTSTRUCT ps;
    pixels      x,y;
    cell_ptr    cell;

    InvalidateRect( WinHandle, NULL, TRUE );
    BeginPaint( WinHandle, &ps );
    SelectObject( ps.hdc, Pen );
    for( x = 0; x < ArraySizeX; ++x ) {
        for( y = 0; y < ArraySizeY; ++y ) {
            cell = CellPointer( x, y );
            if( cell->alive ) {
                BlitBitMap( ps.hdc, x, y );
                cell->drawn = 1;
            }
        }
    }
    if( DrawGrid && !IsAnIcon ) {
        for( x = 0; x < ArraySizeX; ++x ) {
            MoveTo( ps.hdc, x*BitInfo.bmWidth, 0 );
            LineTo( ps.hdc, x*BitInfo.bmWidth, WindowHeight );
        }
        for( y = 0; y < ArraySizeX; ++y ) {
            MoveTo( ps.hdc, 0, y*BitInfo.bmHeight );
            LineTo( ps.hdc, WindowWidth, y*BitInfo.bmHeight );
        }
        MoveTo( ps.hdc, 0, 0 );
        LineTo( ps.hdc, 0, WindowHeight );
        LineTo( ps.hdc, WindowWidth, WindowHeight );
        LineTo( ps.hdc, WindowWidth, 0 );
        LineTo( ps.hdc, 0, 0 );
    }
    EndPaint( WinHandle, &ps );
    FlipSelect();
}
