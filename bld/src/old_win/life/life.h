#include "windows.h"
#include "rules.h"
#include "menu.h"

BOOL GetFileName( char *name, int type, char *ext, char *buff, int len );

#ifdef __WINDOWS_386__
#define  _EXPORT
#define _FAR
#define _free           free
#define _malloc         malloc
#define _hfree          free
#define _halloc( a,b )  malloc( a * b )
#define _memset         memset
#define _memcpy         memcpy
#else
#define  _EXPORT        __export
#define _FAR            __far
#define _free           _ffree
#define _malloc         _fmalloc
#define _hfree          hfree
#define _halloc(a,b)    halloc(a,b)
#define _memset         _fmemset
#define _memcpy         _fmemcpy
#endif

#define ONE_SECOND              1000
#define BUFLEN                  30
#define TIMER_ID                1
#define BUFSIZE                 256

typedef int pixels;

/*
 * Define our cell array data types.
 * NB: The macros let us change the implementation
 */

typedef struct {
    unsigned char alive : 1;
    unsigned char drawn : 1;
    unsigned char next_alive : 1;
} cell_type;

typedef cell_type _FAR *cell_ptr;
typedef cell_type _FAR *cell_ydim;
typedef cell_ydim *cell_array;

#define ArrayPointer( a, x, y )         (&((a)[ (x)+1 ][ (y)+1 ]))
#define CellPointer( x, y )             ArrayPointer( CellArray, x, y )

#ifdef DEFINE_GLOBAL_VARS
    #define GLOBAL
#else
    #define GLOBAL extern
#endif

GLOBAL pixels           WindowWidth;    /* window width in pixels */
GLOBAL pixels           WindowHeight;   /* window height in pixels */
GLOBAL pixels           ScreenHeight;   /* screen height in pixels */
GLOBAL pixels           ArraySizeX;     /* The X dimension of the cell array */
GLOBAL pixels           ArraySizeY;     /* The Y dimension of the cell array */
GLOBAL HANDLE           ThisInst;       /* Our instance handle */
GLOBAL HWND             WinHandle;      /* Out window handle */
GLOBAL int              Mode;           /* (pause, resume, single step ) */
GLOBAL int              MouseMode;      /* (select, flip, single step) */
GLOBAL BOOL             Births[9];      /* Birth rules */
GLOBAL BOOL             Deaths[9];      /* Death rules */
GLOBAL long             Generation;     /* generation counter */
GLOBAL long             Population;     /* population counter */
GLOBAL int              NumberPatterns; /* The number of patterns in &Pattern */
GLOBAL char             Buffer[BUFSIZE];/* Utility buffer */
GLOBAL BOOL             DrawGrid;       /* Do we draw a grid ? */
GLOBAL BOOL             IsAnIcon;       /* Are we an icon? */
GLOBAL BITMAP           BitInfo;        /* Info re: our cell bitmap */
GLOBAL HPEN             Pen;            /* A Black pen */
GLOBAL HBRUSH           Brush;          /* A Hollow brush */
GLOBAL cell_array       CellArray;      /* The array of cells */
GLOBAL BOOL             CurvedSpace;    /* Is space curved? (edges wrap) */

/* From ARRAY.C */

extern BOOL ReSizeArray( pixels width, pixels height, WORD type );
extern void WrapAround( pixels *x, pixels *y );
extern void Clear( void );
extern void Randomize( void );
extern void FreeArray( cell_array junk );
extern void NextGeneration( void );

/* From BITMAPS.C */

extern void BlitBitMap( HDC dc, pixels x, pixels y );
extern void FiniBitMap( void );
extern void InitBitMap( void );
extern void LoadNewBitmap( void );
extern void NewBitMap( void );
extern void UnBlitBitMap( HDC dc, pixels x, pixels y );

/* From LIFE.C */

extern BOOL NoMemory( void );
extern BOOL TurnOffCell( HDC dc, cell_ptr cell, pixels x, pixels y );
extern BOOL TurnOnCell( HDC dc, cell_ptr cell, pixels x, pixels y );
extern void Error( char *str );
extern void FlushMouse( void );
extern void SetCaption( void );

/* from MOUSE.C */

extern BOOL EndSelect( pixels x, pixels y );
extern BOOL MoveSelect( unsigned state, pixels x, pixels y );
extern BOOL SelectOn( void );
extern BOOL StartSelect( pixels x, pixels y );
extern void FlipPattern( unsigned state, pixels x, pixels y );
extern void FlipSelect( void );
extern void GetSelectedCoords( pixels *x1, pixels *x2, pixels *y1, pixels *y2 );
extern void SelectOff( void );

/* From PATTERNS.C */

extern BOOL IsCurrPattern( int i );
extern BOOL ReadAPatternFile( char *name, int i );
extern BOOL ReadPatterns( void );
extern void CreatePatternMenu( void );
extern void DrawPattern( pixels pixel_x, pixels pixel_y, BOOL erase );
extern void FreePatterns( void );
extern void LoadNewPattern( void );
extern void ReflectAboutXequalY( char *pattern );
extern void ReflectAboutYAxis( char *pattern );
extern void SetCurrPattern( int i );
extern void TransformPatterns( void (*rtn)(char *) );
extern void WritePatternFile( void );

/* From TIMER.C */

extern BOOL InitTimer( void );
extern void CheckTimerRate( void );
extern void FiniTimer( void );
extern void NewTimer( void );
extern void RelinquishControl( void );
extern void TimerTurbo( void );
