/*
    ACCEL header file
*/
#define IDR_ACCEL   1
/*
    Menu items
*/
#define IDM_FILE    10
#define IDM_CREATE  100
#define IDM_DELETE  101
#define IDM_ABOUT   102
/*
    Dialog boxes
*/
#define IDD_ABOUT   1
#define IDD_CREATE  2
#define IDD_HELP    3
/*
    IDD_CREATE dialog items
*/
#define IDD_ENTRY   200
#define IDD_CHAR    201
#define IDD_VKEY    202
#define IDD_SCANCODE    203
#define IDD_SHIFT   204
#define IDD_CONTROL 205
#define IDD_ALT     206
#define IDD_LONEKEY 207
#define IDD_CMD     208
#define IDD_SYSCMD  209
#define IDD_FHELP   210
/*
    Prototypes
*/
MRESULT EXPENTRY  AccelWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY  AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY CreateDlgProc(HWND, USHORT, MPARAM, MPARAM);
void cdecl main(void);
