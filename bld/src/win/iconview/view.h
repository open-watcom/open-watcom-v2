#define IDM_FIRSTCHILD      500

#define IDM_OPEN            100
#define IDM_EXIT            101
#define IDM_ABOUT           102
#define IDM_TILE            103
#define IDM_CASCADE         104
#define IDM_ARRANGE         105
#define IDM_SWITCHICON      106
#define IDM_CLOSE           107

/*
 * NOTE: to make some code in view.c easier to write cleanly, these constants
 * are defined to be the same as their counterparts (without IDM_) in
 * windows.h. Due to the #ifndef GDI.. stuff, an RC file cannot use these
 * stretch mode constants. If you have a strong objection to this, the
 * code in view.c will be easy to change, just larger and uglier.
 */

#define IDM_BLACKONWHITE    1
#define IDM_WHITEONBLACK    2
#define IDM_COLORONCOLOR    3

#define FILE_MENU_POS       0           /* not used */
#define OPTION_MENU_POS     1
#define WINDOW_MENU_POS     2

extern HWND            FrameWindow;    /* The classic MDI frame window     */
extern HWND            ClientWindow;   /* And the client window attached   */
extern HANDLE          Instance;       /* Convenient to have handy         */

#ifdef __WINDOWS_386__
#define _EXPORT
#else
#define _EXPORT __export
#endif
