#ifdef __WINDOWS_386__
#define _EXPORT
#else
#define _EXPORT __export
#endif

 /*
  * extra data that is added to each instance of the main window
  * this data can then be used throughout the program by using
  * GetWindowLong
  * this eliminates the need for many global variables
  */
typedef struct {
    BOOL            sound_on;
    BOOL            score_on;
    POINT           target;
    POINT           size;
    POINT           aim;
    POINT           bolt;
    unsigned short  target_speed;
    unsigned short  bolt_speed;
    RECT            client_rect;
    FARPROC         message_window_proc;
    FARPROC         score_window_proc;
    HICON           bolt_icon;
    HBITMAP         target_bmp;
} extra_data;

/* structure to pass all needed data to the DrawBolt function */
typedef struct {
    HICON           icon;
    POINT           size;
    POINT           last;
    HDC             screen_dc;
    HDC             storage_dc;
    short           speed;
} icon_mover;

#define EXTRA_DATA_OFFSET       0

#define MENU_ABOUT              1
#define MENU_EXIT               2
#define MENU_MESSAGE_WINDOW_ON  3
#define MENU_SET_TARGET_SPEED   4
#define MENU_SET_BOLT_SPEED     5
#define MENU_SCORE_WINDOW_ON    6
#define MENU_SOUND_ON           7
#define TARGET_TIMER            1

#define WM_MOVE_TARGET          ( WM_USER + 1 )

/*
 * RGB values for various colours
 * rings are: White ( outside: 5 ), Black, Blue, Red, Yellow ( center: 1 )
 */

#define BACKGROUND              ( 0x800000 )
#define RING5                   ( 0xFFFFFF )
#define RING4                   ( 0x0 )
#define RING3                   ( 0xFF0000 )
#define RING2                   ( 0xFF )
#define RING1                   ( 0xFFFF )

#define BOLTWIDTH               32
#define BOLTHEIGHT              32

/* constants for speed dialog box */
#define SPEED_SCROLL            101
#define SLOW_TEXT               102
#define FAST_TEXT               103
#define SET_WHAT                104
#define TEST                    105

#define SET_TARGET_SPEED        1
#define SET_BOLT_SPEED          2

/* target speeds - the number of milliseconds the target stands still */
#define STD_TARGET_SPEED        1000     /* twice per second */
#define SLOWEST_TARGET_SPEED    5000     /* every 5 seconds */
#define FASTEST_TARGET_SPEED    100      /* 10 times per second */

/* number of pixels to jump before redrawing lightning bolt */
#define STD_BOLT_SPEED          10
#define SLOWEST_BOLT_SPEED      1
#define FASTEST_BOLT_SPEED      32


/* constants for score dialog box */
#define SHOTS 100
#define YELLOW 101
#define RED 102
#define BLUE 103
#define BLACK 104
#define WHITE 105
#define MISSED 106

/* constants for message window dialog box */
#define MOUSE_TITLE                 200
#define MOVMENT_TITLE               201
#define L_BUTTON_TITLE              202
#define R_BUTTON_TITLE              203
#define KEYBOARD_MSG_TITLE          204
#define TIMER_MSG_TITLE             205
#define SCROLL_MSG_TITLE            206
#define MOUSE_MOVE_BOX              210
#define L_BUTTON_BOX                211
#define R_BUTTON_BOX                212
#define KEY_UP_OR_DOWN_BOX          213
#define CHARACTER_BOX               214
#define TIMER_BOX                   215
#define SCROLL_BOX                  216
#define GEN_MSG_BOX                 217
#define MOUSE_ICON                  221
#define KEYBOARD_ICON               223
#define TIMER_ICON                  225
#define SCROLL_ICON                 227
#define FRAME                       230
