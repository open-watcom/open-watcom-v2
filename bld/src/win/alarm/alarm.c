#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "alarm.h"

HANDLE          MyInstance;
char            AlarmClass[32]="AlarmClass";

digit_index     NumberOfDigits = DIGITS_WITH_SECONDS;
digit_index     DigitsToDraw;

unsigned        ScreenWidthInMM;
unsigned        ScreenHeightInMM;

/* following variables have units in Pixels */
pixels  ScreenWidth;
pixels  ScreenHeight;
pixels  WidthOfClock;
pixels  HeightOfClock;
pixels  WidthOfWindow;
pixels  HeightOfWindow;
pixels  InterSegmentGap;
pixels  SegmentHeight;
pixels  SegmentWidth;

char            Buffer[BUFLEN];
digit           ClockDigits[DIGITS_WITH_SECONDS];
unsigned        AlarmDigits[DIGITS_WITH_SECONDS];
colon           Colons[NUMBER_OF_COLONS];
BOOL            AlarmPM = FALSE;
HPEN            Pens[NUMBER_OF_PENS];
BOOL            FirstAlarmSetting = TRUE;
BOOL            TwelveHour = TRUE;
BOOL            Setting;
unsigned        Tick;

static BOOL     AlarmIsRinging;
static BOOL FirstInstance( HANDLE this_inst );
static BOOL AnyInstance( HANDLE this_inst, int cmdshow );

int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst,
                    LPSTR cmdline, int cmdshow )
/***********************************************

    Initialization, message loop.
*/
{
    MSG         msg;

    cmdline = cmdline;
    MyInstance = this_inst;
#ifdef __WINDOWS_386__
    sprintf( AlarmClass,"AlarmClass%d", this_inst );
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

long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

static BOOL FirstInstance( HANDLE this_inst )
/********************************************

    Register window class for the application,
    and do any other application initialization.
*/
{
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = this_inst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "AlarmMenu";
    wc.lpszClassName = AlarmClass;
    rc = RegisterClass( &wc );
    return( rc );

}

static BOOL AnyInstance( HANDLE this_inst, int cmdshow )
/*******************************************************

    Do work required for every instance of the application:
    create the window, initialize data.
*/
{
    HDC         dc;
    HWND        win_handle;

    /*
     * create main window
     */

    dc = GetDC(NULL);
    ScreenHeight = GetDeviceCaps( dc, VERTRES );
    ScreenWidth = GetDeviceCaps( dc, HORZRES );
    ScreenHeightInMM = GetDeviceCaps( dc, VERTSIZE );
    ScreenWidthInMM = GetDeviceCaps( dc, HORZSIZE );
    ReleaseDC( NULL, dc );
    CreateSupplies();

    win_handle = CreateWindow(
        AlarmClass,             /* class */
        "Alarm Clock",          /* caption */
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

    if( !win_handle ) return( FALSE );

    /*
     * display window
     */
    ShowWindow( win_handle, cmdshow );
    UpdateWindow( win_handle );

    if( !SetTimer( win_handle, TIMER_ID, ONE_SECOND/4, 0L ) ) {
        MessageBox( NULL, "Too many timers in use", Buffer,
                   MB_ICONHAND+MB_OK+MB_SYSTEMMODAL );
        return( FALSE );
    }

    return( TRUE );

}

BOOL _EXPORT FAR PASCAL About( HWND win_handle, unsigned msg,
                                WORD wparam, LONG lparam )
/*********************************************************

    Process messages for the about dialog.
*/
{
    lparam = lparam;                    /* turn off warning */
    win_handle = win_handle;

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK ) {
            EndDialog( win_handle, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

}


static void TransferClockToAlarm()
/*********************************

    Transfer the digits from the Clock array to the Alarm array.
*/
{
    digit_index         i;

    for( i = 0; i < DIGITS_WITH_SECONDS; ++i ) {
        AlarmDigits[i] = ClockDigits[i].value;
    }
}

static void TransferAlarmToClock()
/*********************************

    Transfer digits from the Alarm array to the Clock array
*/
{
    digit_index         i;

    for( i = 0; i < DIGITS_WITH_SECONDS; ++i ) {
        ClockDigits[i].value = AlarmDigits[i];
    }
}

static void RePaintTheClock( HWND win_handle )
/*********************************************

    Re-paint the entire clock, since it has been invalidated.
*/
{
    PAINTSTRUCT ps;

    InvalidateRect( win_handle, NULL, TRUE );
    BeginPaint( win_handle, &ps );
    InitializeTheClock();
    PaintClock( ps.hdc );
    EndPaint( win_handle, &ps );
}


static void CheckAlarm()
/***********************

    See if the alarm has gone off, and pop up a message box if it has.
*/
{
    digit_index         i;

    if( AlarmIsRinging ) return;
    for( i = 0; i < DIGITS_WITH_SECONDS; ++i ) {
        if( AlarmDigits[i] != ClockDigits[i].value ) return;
    }
    AlarmIsRinging = TRUE;
    MessageBox( NULL, "The alarm clock is ringing!", Buffer,
               MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL );
    AlarmIsRinging = FALSE;
}


static void UpdateTheClock( HWND win_handle )
/********************************************

    Incrementally update the clock display.
*/
{
    HDC         dc;

    dc = GetDC( win_handle );
    PaintClock( dc );
    ReleaseDC( win_handle, dc );
}


static void SetNumberOfDigits( digit_index num )
/***********************************************

    Set the number of digits to be displayed (4 or 6)
*/
{
    NumberOfDigits = num;
    DigitsToDraw = num;
}


static void SetAlarm()
/*********************

    Put the clock into alarm setting mode.
*/
{
    if( Setting ) return;
    Setting = TRUE;
    if( FirstAlarmSetting ) {
        TransferClockToAlarm();
        if( NumberOfDigits == DIGITS_WITHOUT_SECONDS ) {
            AlarmDigits[SEC_TENS] = 0;
            AlarmDigits[SEC_ONES] = 0;
        }
        FirstAlarmSetting = FALSE;
    }
    TransferAlarmToClock();
}

static void RunTheClock()
/************************

    Put the clock into running mode.
*/
{
    if( !Setting ) return;
    Setting = FALSE;
    TransferClockToAlarm();
}


static void Set12HourClock( HWND win_handle )
/********************************************

    Put the clock into 12 hour mode.
*/
{
    int         i;

    if( TwelveHour ) return;
    TwelveHour = TRUE;
    i = AlarmDigits[HOUR_TENS]*10 + AlarmDigits[HOUR_ONES];
    if( i > 12 ) {
        i -= 12;
        AlarmPM = TRUE;
    }
    AlarmDigits[HOUR_TENS] = i / 10;
    AlarmDigits[HOUR_ONES] = i % 10;
    if( Setting ) TransferAlarmToClock();
    UpdateTheClock( win_handle );
}


static void Set24HourClock( HWND win_handle )
/********************************************

    Put the clock into 24 hour mode
*/
{
    int         i;

    if( !TwelveHour ) return;
    TwelveHour = FALSE;
    i = AlarmDigits[HOUR_TENS]*10 + AlarmDigits[HOUR_ONES];
    if( i <= 12 && AlarmPM ) {
        i += 12;
        AlarmPM = FALSE;
    }
    AlarmDigits[HOUR_TENS] = i / 10;
    AlarmDigits[HOUR_ONES] = i % 10;
    if( Setting ) TransferAlarmToClock();
    UpdateTheClock( win_handle );
}


static void DisplayAboutBox( HWND win_handle )
/*********************************************

    Display the "About ..." box
*/
{
    FARPROC     proc;

    proc = MakeProcInstance( About, MyInstance );
    DialogBox( MyInstance, "AboutBox", win_handle, proc );
    FreeProcInstance( proc );
}


static void MenuItem( HWND win_handle, WORD wparam )
/***************************************************

    Handle a menu item which has been selected by the user.
*/
{

    switch( wparam ) {
    case MENU_ABOUT:
        DisplayAboutBox( win_handle );
        break;
    case MENU_DISPLAY_SECONDS:
        SetNumberOfDigits( DIGITS_WITH_SECONDS );
        break;
    case MENU_SUPRESS_SECONDS:
        SetNumberOfDigits( DIGITS_WITHOUT_SECONDS );
        break;
    case MENU_SET_ALARM:
        SetAlarm();
        break;
    case MENU_RUN_CLOCK:
        RunTheClock();
        break;
    case MENU_12_HOUR_CLOCK:
        Set12HourClock( win_handle );
        break;
    case MENU_24_HOUR_CLOCK:
        Set24HourClock( win_handle );
        break;
    }
    InvalidateRect( win_handle, NULL, TRUE );
}


static  int Check[] = { MF_UNCHECKED, MF_CHECKED };

static void InitMenu( HMENU mh )
/*******************************

    Initialize the menu display. Disable any items which are not applicable.
*/
{
    BOOL        on;

    CheckMenuItem( mh, MENU_SET_ALARM, Check[ Setting ] );
    CheckMenuItem( mh, MENU_RUN_CLOCK, Check[ !Setting ] );
    on = ( NumberOfDigits == DIGITS_WITHOUT_SECONDS );
    CheckMenuItem( mh, MENU_SUPRESS_SECONDS, Check[ on ] );
    CheckMenuItem( mh, MENU_DISPLAY_SECONDS, Check[ !on ] );
    CheckMenuItem( mh, MENU_24_HOUR_CLOCK, Check[ !TwelveHour ] );
    CheckMenuItem( mh, MENU_12_HOUR_CLOCK, Check[ TwelveHour ] );
}

static unsigned  MaximumNumber[] = { 0, 0, 5, 9, 5, 9 };

static void MouseClick( pixels x, pixels y, unsigned msg )
/*********************************************************

    Handle a mouse click at position x,y.
*/
{
    digit_index         i;

    TransferAlarmToClock();
    if( TwelveHour ) {
        MaximumNumber[HOUR_TENS] = 1;
        if( ClockDigits[HOUR_TENS].value == 1 ) {
            MaximumNumber[HOUR_ONES] = 2;
        } else {
            MaximumNumber[HOUR_ONES] = 9;
        }
    } else {
        MaximumNumber[HOUR_TENS] = 2;
        if( ClockDigits[HOUR_TENS].value == 2 ) {
            MaximumNumber[HOUR_ONES] = 4;
        } else {
            MaximumNumber[HOUR_ONES] = 9;
        }
    }
    for( i = 0; i < NumberOfDigits; ++i ) {
        if( x < ClockDigits[i].segment[SEGMENT_0].position.start.x ) continue;
        if( x > ClockDigits[i].segment[SEGMENT_6].position.end.x ) continue;
        if( y < ClockDigits[i].segment[SEGMENT_0].position.start.y ) continue;
        if( y > ClockDigits[i].segment[SEGMENT_6].position.end.y ) continue;
        if( msg == WM_LBUTTONDOWN ) {
            ClockDigits[i].value++;
        } else if( msg == WM_RBUTTONDOWN ) {
            ClockDigits[i].value--;
        }
        if( ClockDigits[i].value > MaximumNumber[ i ] ) {
            ClockDigits[i].value = 0;
        } else if( ClockDigits[i].value < 0 ) {
            ClockDigits[i].value = MaximumNumber[i];
        }
    }
    if( TwelveHour ) {
        if( x < ClockDigits[HOUR_TENS].segment[SEGMENT_0].position.start.x &&
            y > ClockDigits[HOUR_TENS].segment[SEGMENT_0].position.start.y &&
            y < ClockDigits[HOUR_TENS].segment[SEGMENT_6].position.end.y ) {
            AlarmPM = !AlarmPM;
        }
        if( ClockDigits[HOUR_TENS].value == 1 && ClockDigits[HOUR_ONES].value > 2 ) {
            ClockDigits[HOUR_ONES].value = 2;
        }
        if( ClockDigits[HOUR_TENS].value == 0 &&
            ClockDigits[HOUR_ONES].value == 0 ) {
            ClockDigits[HOUR_ONES].value = 1;
        }
    } else {
        if( ClockDigits[HOUR_TENS].value == 2 &&
            ClockDigits[HOUR_ONES].value > 4 ) {
            ClockDigits[HOUR_ONES].value = 4;
        }
    }
    TransferClockToAlarm();
}


LONG _EXPORT FAR PASCAL WindowProc( HWND win_handle, unsigned msg,
                                     UINT wparam, LONG lparam )
/**************************************************************

    Handle messages for the main application window.
*/
{

    switch( msg ) {
    case WM_INITMENU:
        InitMenu( (HMENU)wparam );
        break;

    case WM_COMMAND:
        MenuItem( win_handle, LOWORD(wparam) );
        break;

    case WM_SYSCOLORCHANGE:
        DeleteSupplies();
        CreateSupplies();
        break;

    case WM_PAINT:
        RePaintTheClock( win_handle );
        break;

    case WM_TIMER:
    case WM_TIMECHANGE:
        if( Setting ) break;
        Tick++;
        CheckAlarm();
        UpdateTheClock( win_handle );
        break;

    case WM_SIZE:
        ReSize( LOWORD( lparam ), HIWORD( lparam ), wparam );
        break;

    case WM_DESTROY:
        DeleteSupplies();
        KillTimer( win_handle, TIMER_ID );
        PostQuitMessage( 0 );
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        if( !Setting ) break;
        MouseClick( LOWORD( lparam ), HIWORD( lparam ), msg );
        UpdateTheClock( win_handle );
        break;

    default:
        return( DefWindowProc( win_handle, msg, wparam, lparam ) );
    }
    return( 0L );

}



static seg_position  SegPosition[ NUMBER_OF_SEGMENTS ] =
/*******************************************************

    These are the positions of the start and end points
    of each digit segment segments based on the
    following co-ordinate system.


   (0,0) ---0--- (1,0)
        |       |
        1       2
        |       |
   (0,1) ---3--- (1,1)
        |       |
        4       5
        |       |
   (0,2) ---6--- (1,2)

*/
    /* start.x  start.y end.x   end.y */
{
    {  0,       0,      1,      0 },    /* 0 */
    {  0,       0,      0,      1 },    /* 1 */
    {  1,       0,      1,      1 },    /* 2 */
    {  0,       1,      1,      1 },    /* 3 */
    {  0,       1,      0,      2 },    /* 4 */
    {  1,       1,      1,      2 },    /* 5 */
    {  0,       2,      1,      2 },    /* 6 */
};

/*************************************************************************

    The following arrays define the polygon for each digit segment
    based on the two end points defined in SegPosition. Each entry
    is an (x,y) point based on the start or end point of the line
    segment plus some delta in the x and y direction. For example,
    START+UP_QUARTER+RITE_HALF means the starting point of the line
    segment, moved up 1/4 of a segment width, and rite 1/2 of a
    segment width
*/

typedef enum {
        START           = 0x0001,
        END             = 0x0002,
        DOWN_ONE        = 0x0004,
        UP_ONE          = 0x0008,
        DOWN_HALF       = 0x0010,
        UP_HALF         = 0x0020,
        DOWN_QUARTER    = 0x0040,
        UP_QUARTER      = 0x0080,
        LEFT_ONE        = 0x0100,
        RITE_ONE        = 0x0200,
        LEFT_HALF       = 0x0400,
        RITE_HALF       = 0x0800,
        LEFT_QUARTER    = 0x1000,
        RITE_QUARTER    = 0x2000
} point_delta;

static point_delta Poly0[] = {
        START+UP_QUARTER+RITE_QUARTER,
        END+UP_QUARTER+LEFT_QUARTER,
        END,
        END+DOWN_ONE+LEFT_ONE,
        START+DOWN_ONE+RITE_ONE,
        START,
        0
};

static point_delta Poly1[] = {
        START+DOWN_QUARTER+LEFT_QUARTER,
        END+UP_QUARTER+LEFT_QUARTER,
        END,
        END+UP_HALF+RITE_ONE,
        START+DOWN_ONE+RITE_ONE,
        START,
        0
};

static point_delta Poly2[] = {
        START+RITE_QUARTER+DOWN_QUARTER,
        END+UP_QUARTER+RITE_QUARTER,
        END,
        END+UP_HALF+LEFT_ONE,
        START+DOWN_ONE+LEFT_ONE,
        START,
        0
};

static point_delta Poly3[] = {
        START+UP_HALF+RITE_ONE,
        END+UP_HALF+LEFT_ONE,
        END,
        END+DOWN_HALF+LEFT_ONE,
        START+DOWN_HALF+RITE_ONE,
        START,
        0
};

static point_delta Poly4[] = {
        START+DOWN_QUARTER+LEFT_QUARTER,
        END+UP_QUARTER+LEFT_QUARTER,
        END,
        END+UP_ONE+RITE_ONE,
        START+DOWN_HALF+RITE_ONE,
        START,
        0
};

static point_delta Poly5[] = {
        START+RITE_QUARTER+DOWN_QUARTER,
        END+UP_QUARTER+RITE_QUARTER,
        END,
        END+UP_ONE+LEFT_ONE,
        START+DOWN_HALF+LEFT_ONE,
        START,
        0
};

static point_delta Poly6[] = {
        START+DOWN_QUARTER+RITE_QUARTER,
        END+DOWN_QUARTER+LEFT_QUARTER,
        END,
        END+UP_ONE+LEFT_ONE,
        START+UP_ONE+RITE_ONE,
        START,
        0
};

static point_delta *SegPoly[] =
{
    &Poly0,  &Poly1,  &Poly2,  &Poly3,  &Poly4,  &Poly5,  &Poly6
};


static void DrawSegment( HDC dc, pen_type pen,
                         seg_position *pos, point_delta *delta )
/***************************************************************

    Draw the polygon for a segment of a digit.
*/
{
    pixels      x,y;
    pixels      one_x,one_y;
    pixels      half_x,half_y;
    pixels      quarter_x,quarter_y;

    one_y = SegmentHeight;
    half_y = SegmentHeight / 2;
    quarter_y = SegmentHeight / 4;
    if( one_y <= 2 ) one_y = 1;
    one_x = SegmentWidth;
    half_x = SegmentWidth / 2;
    quarter_x = SegmentWidth / 4;
    if( one_x <= 2 ) one_x = 1;
    SelectObject( dc, Pens[pen] );
    MoveToEx( dc, pos->start.x, pos->start.y, NULL );
    while( *delta != 0 ) {
        if( *delta & START ) {
            x = pos->start.x;
            y = pos->start.y;
        } else if( *delta & END ) {
            x = pos->end.x;
            y = pos->end.y;
        }
        if( *delta & UP_QUARTER )       y -= quarter_y;
        if( *delta & UP_HALF )          y -= half_y;
        if( *delta & UP_ONE )           y -= one_y;
        if( *delta & DOWN_QUARTER )     y += quarter_y;
        if( *delta & DOWN_HALF )        y += half_y;
        if( *delta & DOWN_ONE )         y += one_y;
        if( *delta & LEFT_QUARTER )     x -= quarter_x;
        if( *delta & LEFT_HALF )        x -= half_x;
        if( *delta & LEFT_ONE )         x -= one_x;
        if( *delta & RITE_QUARTER )     x += quarter_x;
        if( *delta & RITE_HALF )        x += half_x;
        if( *delta & RITE_ONE )         x += one_x;
        LineTo( dc, x, y );
        ++delta;
    }
    LineTo( dc, pos->start.x, pos->start.y );
}


static BOOL SegmentOnInDigit[ 11 ][ NUMBER_OF_SEGMENTS ] =
/*********************************************************

    SegmentIsOn[ i ][ j ] indicates whether or not segment number i should
    be turned on in the digit j.
*/
{
/*    0      1      2      3      4      5      6        */
    {  TRUE,  TRUE,  TRUE, FALSE,  TRUE,  TRUE,  TRUE }, /* 0 */
    { FALSE, FALSE,  TRUE, FALSE, FALSE,  TRUE, FALSE }, /* 1 */
    {  TRUE, FALSE,  TRUE,  TRUE,  TRUE, FALSE,  TRUE }, /* 2 */
    {  TRUE, FALSE,  TRUE,  TRUE, FALSE,  TRUE,  TRUE }, /* 3 */
    { FALSE,  TRUE,  TRUE,  TRUE, FALSE,  TRUE, FALSE }, /* 4 */
    {  TRUE,  TRUE, FALSE,  TRUE, FALSE,  TRUE,  TRUE }, /* 5 */
    {  TRUE,  TRUE, FALSE,  TRUE,  TRUE,  TRUE,  TRUE }, /* 6 */
    {  TRUE,  TRUE,  TRUE, FALSE, FALSE,  TRUE, FALSE }, /* 7 */
    {  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE }, /* 8 */
    {  TRUE,  TRUE,  TRUE,  TRUE, FALSE,  TRUE,  TRUE }, /* 9 */
    { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE }, /* 10 */
};

static void DrawDigit( HDC dc, digit *p )
/****************************************

    Draw one digit of the the clock.
*/
{
    segment_index               i;

    for( i = 0; i < NUMBER_OF_SEGMENTS; ++i ) {
        if( SegmentOnInDigit[ p->value ][ i ] ) {
            if( !p->segment[i].on ) {
                DrawSegment( dc, FOREGROUND,
                             &p->segment[i].position, SegPoly[i] );
                p->segment[i].on = TRUE;
            }
        } else {
            if( p->segment[i].on ) {
                DrawSegment( dc, BACKGROUND,
                             &p->segment[i].position, SegPoly[i] );
                p->segment[i].on = FALSE;
            }
        }
    }
}


static void UpdateColon( HDC dc, colon_index i, pen_type *pen )
/**************************************************************

    Draw the colon separating the clock digits (and the AM/PM indicator)
*/
{
    dot_index           j;

    for( j = 0; j < NUMBER_OF_DOTS; ++j ) {
        if( Colons[i].dot[j].pen != pen[j] ) {
            SelectObject( dc, Pens[ pen[j] ] );
            Rectangle( dc, Colons[i].dot[j].top_left.x,
                           Colons[i].dot[j].top_left.y,
                           Colons[i].dot[j].bot_rite.x,
                           Colons[i].dot[j].bot_rite.y );
            Colons[i].dot[j].pen = pen[j];
        }
    }
}


static void DrawColon( HDC dc, colon_index i )
/*********************************************

    Draw the colon separating the digits. It flashes if seconds are supressed
*/
{
    pen_type    pen[NUMBER_OF_DOTS];

    pen[TOP_DOT] = FOREGROUND;
    if( !Setting && ( DigitsToDraw == DIGITS_WITHOUT_SECONDS ) &&
        ( ( Tick % 4 ) < 2 ) ) {
        pen[TOP_DOT] = BACKGROUND;
    }
    pen[BOT_DOT] = pen[TOP_DOT];
    UpdateColon( dc, i, pen );
}


static void DrawPM( HDC dc, BOOL pm )
/************************************

    Draw the AM/PM indicator (This is just a colon in front of the first digit
*/
{
    pen_type    pen[NUMBER_OF_DOTS];

    if( pm ) {
        pen[TOP_DOT] = BACKGROUND;
        pen[BOT_DOT] = FOREGROUND;
    } else {
        pen[BOT_DOT] = BACKGROUND;
        pen[TOP_DOT] = FOREGROUND;
    }
    UpdateColon( dc, 0, pen );
}


static void PaintClock( HDC dc )
/*******************************

    Paint the clock face on the screen
*/
{
    digit_index i;
    time_t      ltime;
    struct tm   *t;
    BOOL        pm;

    pm = FALSE;
    if( !Setting ) {
        time( &ltime );
        t = localtime( &ltime );
        if( TwelveHour ) {
            if( t->tm_hour > 12 ) {
                t->tm_hour -= 12;
                pm = TRUE;
            } else if( t->tm_hour == 0 ) {
                t->tm_hour = 12;
            }
        }
        ClockDigits[HOUR_TENS].value = t->tm_hour / 10;
        ClockDigits[HOUR_ONES].value = t->tm_hour % 10;
        ClockDigits[MIN_TENS].value = t->tm_min / 10;
        ClockDigits[MIN_ONES].value = t->tm_min % 10;
        ClockDigits[SEC_TENS].value = t->tm_sec / 10;
        ClockDigits[SEC_ONES].value = t->tm_sec % 10;
    } else {
        pm = AlarmPM;
    }
    if( ClockDigits[HOUR_TENS].value == 0 ) {
        ClockDigits[HOUR_TENS].value = BLANK_SPACE_VALUE;
    }
    for( i = 0; i < DigitsToDraw; ++i ) {
        DrawDigit( dc, &ClockDigits[i] );
    }
    DrawColon( dc, 1 );
    if( DigitsToDraw == DIGITS_WITH_SECONDS ) {
        DrawColon( dc, 2 );
    }
    if( TwelveHour ) {
        DrawPM( dc, pm );
    }
    if( ClockDigits[HOUR_TENS].value == BLANK_SPACE_VALUE ) {
        ClockDigits[HOUR_TENS].value = 0;
    }
}


static void SetColonPos( colon_index i )
/***************************************

    Set the screen positions for colon number "i".
*/
{
    pixels      dx,dy;
    pixels      add_x,add_y,sub_x,sub_y;
    pixels      x1,y1,x2,y2;

    y1 = ClockDigits[i].segment[SEGMENT_0].position.start.y;
    y2 = ClockDigits[i].segment[SEGMENT_6].position.start.y;
    switch( i ) {
    case 0:
        x1 = 0;
        x2 = ClockDigits[HOUR_TENS].segment[SEGMENT_0].position.start.x;
        break;
    case 1:
        x1 = ClockDigits[HOUR_ONES].segment[SEGMENT_0].position.end.x;
        x2 = ClockDigits[MIN_TENS].segment[SEGMENT_0].position.start.x;
        break;
    case 2:
        x1 = ClockDigits[MIN_ONES].segment[SEGMENT_0].position.end.x;
        x2 = ClockDigits[SEC_TENS].segment[SEGMENT_0].position.start.x;
        break;
    }
    dx = ( x2 - x1 ) / 2;
    dy = ( y2 - y1 ) / 3;
    add_x = dx / PORTION_FOR_COLON;
    sub_x = -add_x;
    if( add_x == 0 ) add_x = 1;
    add_y = dy / PORTION_FOR_COLON;
    sub_y = -add_y;
    if( add_y == 0 ) add_y = 1;
    x1 += dx;
    y1 += dy;
    Colons[i].dot[TOP_DOT].top_left.x = x1 + sub_x;
    Colons[i].dot[TOP_DOT].top_left.y = y1 + sub_y;
    Colons[i].dot[TOP_DOT].bot_rite.x = x1 + add_x;
    Colons[i].dot[TOP_DOT].bot_rite.y = y1 + add_y;
    Colons[i].dot[TOP_DOT].pen = BACKGROUND;
    y1 += dy;
    Colons[i].dot[BOT_DOT].top_left.x = x1 + sub_x;
    Colons[i].dot[BOT_DOT].top_left.y = y1 + sub_y;
    Colons[i].dot[BOT_DOT].bot_rite.x = x1 + add_x;
    Colons[i].dot[BOT_DOT].bot_rite.y = y1 + add_y;
    Colons[i].dot[BOT_DOT].pen = BACKGROUND;
}


static void InitializeTheClock()
/*******************************

    Initialize each digit of the clock. Calculate the screen position
    of each segment of each digit of the clock, as well as the
    screen positions of the colons.
*/
{
    pixels              win_left,win_rite;
    pixels              top_of_clock;
    pixels              top,bot;
    pixels              left,rite;
    pixels              width,height;
    pixels              space;
    pixels              colon_space;
    digit_index         i;
    segment_index       j;
    long                width_scale;
    long                height_scale;

    HeightOfClock = HeightOfWindow;
    if( HeightOfClock < MINIMUM_HEIGHT ) {
        HeightOfClock = MINIMUM_HEIGHT;
    }
    WidthOfClock = WidthOfWindow;
    height_scale = (long)ScreenHeightInMM * DIGIT_HEIGHT_SCALE;
    width_scale = (long)ScreenWidthInMM * DIGIT_WIDTH_SCALE / ( DigitsToDraw + 1 ); /* Allow for 2 colons */
    if( WidthOfClock * width_scale > HeightOfClock * height_scale ) {
        WidthOfClock = HeightOfClock * height_scale / width_scale;
    } else {
        HeightOfClock = WidthOfClock * width_scale / height_scale;
    }
    top_of_clock = ( HeightOfWindow - HeightOfClock ) / 2;

    top = HeightOfClock / UNUSED_PORTION_OF_SCREEN_HEIGHT;
    bot = HeightOfClock - top;
    height = bot - top;
    top += top_of_clock;

    win_left = WidthOfClock / UNUSED_PORTION_OF_SCREEN_WIDTH;
    win_rite = WidthOfClock - win_left;

    space = ( win_rite - win_left ) / ( DigitsToDraw + 1 );
    InterSegmentGap = space / PORTION_FOR_SEGMENT_GAP;
    SegmentWidth = space / PORTION_OF_SPACE_FOR_SEGMENT;
    SegmentHeight = (long)SegmentWidth * ScreenHeightInMM / ScreenWidthInMM;

    left = space / UNUSED_PORTION_OF_DIGIT_SPACE;
    rite = space - left;
    width = rite - left;

    for( i = 0; i < DigitsToDraw; ++i ) {
        for( j = 0; j < NUMBER_OF_SEGMENTS; ++j ) {
            ClockDigits[i].segment[j].on = FALSE;
            ClockDigits[i].segment[j].position = SegPosition[j];
            /* scale the co-ordinates */
            ClockDigits[i].segment[j].position.start.x *= width;
            ClockDigits[i].segment[j].position.end.x   *= width;
            ClockDigits[i].segment[j].position.start.y *= height / 2;
            ClockDigits[i].segment[j].position.end.y   *= height / 2;
            /* adjust for left edge of digit's bounding box and window */
            colon_space = i / 2 * ( width / 2 ); /* 0,1 -> 0, 2,3 -> 1/2 width, 4,5 -> width */
            ClockDigits[i].segment[j].position.start.x += colon_space + i * space + left + win_left;
            ClockDigits[i].segment[j].position.end.x   += colon_space + i * space + left + win_left;
            /* adjust for top edge of digit's bounding box */
            ClockDigits[i].segment[j].position.start.y += top;
            ClockDigits[i].segment[j].position.end.y   += top;
            /* adjust all edges for gap */
            if( SegPosition[j].start.x != SegPosition[j].end.x ) {
                ClockDigits[i].segment[j].position.start.x += InterSegmentGap;
                ClockDigits[i].segment[j].position.end.x -= InterSegmentGap;
            } else {
                ClockDigits[i].segment[j].position.start.y += InterSegmentGap;
                ClockDigits[i].segment[j].position.end.y -= InterSegmentGap;
            }
        }
    }
    for( i = 0; i < NUMBER_OF_COLONS; ++i ) {
        SetColonPos( i );
    }
}


static void ReSize( pixels width, pixels height, WORD type )
/***********************************************************

    Record the new size of the window and redraw the clock accordingly
*/
{
    WidthOfWindow = width;
    HeightOfWindow = height;
    DigitsToDraw = NumberOfDigits;
    if( type == SIZEICONIC ) {
        DigitsToDraw = DIGITS_WITHOUT_SECONDS;
    } else {
        DigitsToDraw = NumberOfDigits;
    }
    InitializeTheClock();
}

void CreateSupplies()
/********************

    Create the art supplies we need
*/
{
    Pens[FOREGROUND] = CreatePen( 0, 1, GetSysColor( COLOR_WINDOWTEXT ) );
    Pens[BACKGROUND] = CreatePen( 0, 1, GetSysColor( COLOR_WINDOW ) );
}


void DeleteSupplies()
/********************

    Delete our art supplies
*/
{
    DeleteObject( Pens[FOREGROUND] );
    DeleteObject( Pens[BACKGROUND] );
}
