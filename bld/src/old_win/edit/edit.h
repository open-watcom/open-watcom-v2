#define MENU_ABOUT              1
#define MENU_NEW                2
#define MENU_OPEN               3
#define MENU_SAVE               4
#define MENU_SAVE_AS            5
#define MENU_PRINT              6
#define MENU_PRINT_SETUP        7
#define MENU_CLEAR              8
#define MENU_COPY               9
#define MENU_CUT                10
#define MENU_PASTE              11
#define MENU_UNDO               12
#define MENU_EXIT               13
#define MENU_FONT_SELECT        14

#define FONT_NAME               50
#define FONT_SIZE               51
#define FONT_SAMPLE             52

#define EDIT_ID                 100
#define PRINT_TITLE             200
#define PRINT_REST              201

#ifdef __WINDOWS_386__
#define _EXPORT
#define _FAR
typedef void *ALLOCPTR;
#define _strcmp strcmp
#define _memcpy memcpy
#define _strlen strlen
#else
#define _EXPORT __export
typedef void __far *ALLOCPTR;
#define _FAR    __far
#define _strcmp _fstrcmp
#define _strlen _fstrlen
#define _memcpy _fmemcpy
#endif


#define EXTRA_DATA_OFFSET       0
typedef struct {
HWND    hwnd;           /* main window */
HWND    editwnd;        /* edit window */
HANDLE  inst;           /* instance handle */
char    _FAR *filename; /* current filename */
BOOL    needs_saving;   /* indicates edit file has been saved */
HFONT   font;
} extra_data;
typedef extra_data _FAR *LPEDATA;

typedef struct font_info {
struct font_info _FAR *next;
int     index;
int     size_count;
short   _FAR *sizes;
char    name[32];
char    charset;
char    pitch_family;
} font_info;
typedef font_info _FAR *LPFINFO;


typedef enum {
PSUPP_NONE,
PSUPP_CANPRINT,
PSUPP_CANPRINTANDSET
} psupp;

extern char EditTitle[];
extern psupp PrinterSupport;

/*
 * function prototypes
 */
/* efile.c */
BOOL CheckFileSave( LPEDATA );
BOOL FileSave( LPEDATA, BOOL );
void FileEdit( LPEDATA, BOOL );

/* efont.c */
void FontSelect( LPEDATA );
void GetAllFonts( LPEDATA );

/* emem.c */
ALLOCPTR MemAlloc( unsigned );
ALLOCPTR MemRealloc( ALLOCPTR, unsigned );
void MemFree( ALLOCPTR );

/* eprint.c */
HDC PrinterDC( void );
void GetPrinterSetup( HWND );
BOOL Print( LPEDATA );
