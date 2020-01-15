/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <dos.h>
#ifdef __OS2__
    #define INCL_SUB
    #define INCL_DOSQUEUES
    #define INCL_DOSPROCESS
    #define INCL_DOSMISC
    #include <os2.h>
#endif

/* define DOS if compiling for dos */

/* OPTIONS start here */

#define PATH "PATH="
#ifdef __DOS__
    #define     ALIAS_FILE      "CMDEDIT="
    #define     SAVE_SIZE       512
    #define     MAX_FNAME       (256+100) // make sure DIRINFO bigger than WIN32_FIND_DATA
#else
    #define     ALIAS_FILE      "OS2EDIT="
    #define     SAVE_SIZE       4096
    #define     MAX_FNAME       256
#endif
#define ALIAS_SLACK 10

#define MATCH_NAME     "CMDMATCH="

/* OPTIONS end here */

#ifdef __DOS__

#define PASPTR /**/
#define USHORT  unsigned short

enum { FALSE, TRUE };

#else

#define PASPTR __far

#endif

#define KBD_ASCII       0x8
#define KBD_BINARY      0x4
#define KBD_ECHO_ON     0x2
#define KBD_ECHO_OFF    0x1

#define _D_SDIR 0x10

typedef unsigned long DWORD;

typedef struct {
    int     input;
    int     output;
} LENGTH;

#ifdef __DOS__

typedef char            CHAR;
typedef unsigned char   UCHAR;

typedef struct {
    int     cb;
    int     fsMask;
    int     chTurnAround;
    int     interm;
    int     shift;
} KBDDESC;

typedef struct {
    UCHAR   chChar;
    UCHAR   chScan;
    UCHAR   status;
    UCHAR   zero;
    int     shift;
    long    time;
} KBDCHAR;

typedef struct {
    UCHAR   ascii;
    UCHAR   scan;
    UCHAR   status;
    UCHAR   zero;
    int     shift;
} KBDCHARNT;

typedef struct {
    int     yStart;
    int     cEnd;
    int     width;
    int     attr;
} CURSOR;

typedef struct {
    char    dta[21];
    UCHAR   attrFile;
    char    crap[8];
    char    achName[MAX_FNAME];
} DIRINFO;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _WIN32_FIND_DATA {
    DWORD       dwFileAttributes;
    FILETIME    ftCreationTime;
    FILETIME    ftLastAccessTime;
    FILETIME    ftLastWriteTime;
    DWORD       nFileSizeHigh;
    DWORD       nFileSizeLow;
    DWORD       dwReserved0;
    DWORD       dwReserved1;
    CHAR        cFileName[260];
    CHAR        cAlternateFileName[16];
} WIN32_FIND_DATA;

#else

typedef struct _KBDKEYINFO KBDCHAR;

typedef struct _KBDINFO KBDDESC;

typedef struct _VIOCURSORINFO CURSOR;

typedef struct _FILEFINDBUF DIRINFO;

#endif

#ifdef __DOS__
#define LINE_WIDTH 128
#else
#define LINE_WIDTH 512
#endif

#define SCREEN_WIDTH 80
#define MENU_DEPTH 5
#define MENU_SIZE (SCREEN_WIDTH*MENU_DEPTH)

enum {
        BACKSPACE = '\b',
        CTRL_BACKSPACE = 127,
        CTRL_BREAK = 0,
        TAB = 9,
        BACK_TAB = 15,
        ALT_0 = 129,
        ALT_2 = 121,
        ALT_A = 30,
        ALT_B = 48,
        ALT_C = 46,
        ALT_D = 32,
        ALT_E = 18,
        ALT_H = 35,
        ALT_I = 23,
        ALT_K = 37,
        ALT_L = 38,
        ALT_N = 49,
        ALT_P = 25,
        ALT_R = 19,
        ALT_U = 22,
        ALT_X = 45,
        ALT_Z = 44,
        ESCAPE = 27,
        LEFT = 75,
        RIGHT = 77,
        HOME = 71,
        END = 79,
        UP = 72,
        DOWN = 80,
        INSERT = 82,
        CTRL_INSERT = 146,
        DELETE = 83,
        CTRL_LEFT = 115,
        CTRL_RIGHT = 116,
        CTRL_END = 117,
        CTRL_HOME = 119,
        CTRL_DELETE = 147,
        PAGEUP = 73,
        PAGEDOWN = 81,
        CTRL_PAGEUP = 132,
        CTRL_PAGEDOWN = 118,
        CTRL_UP = 141,
        CTRL_DOWN = 145,
        CTRL_TAB = 148,
        ALT_UP = 152,
        ALT_DOWN = 160,
        ALT_LEFT = 155,
        ALT_RIGHT = 157,
        ALT_HOME = 151,
        ALT_END = 159,
        ALT_TAB = 165,
        F1 = 59,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11 = 84,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        F26,
        F27,
        F28,
        F29,
        F30,
        F31,
        F32,
        F33,
        F34,
        F35,
        F36,
        F37,
        F38,
        F39,
        F40,
        I_HATE_COMMA_SEPARATED_LISTS
        };

#define F0 ( F1 - 1 )

#define _white( ch ) ( ( ch ) == ' ' || ( ch ) == '\t' )
#define _null( ch )  ( ( ch ) == '\0' )
#define _white_or_null( ch ) ( _white( ch ) || _null( ch ) )

#ifdef __DOS__

#define DosSetSigHandler( r, p, pa, a, n )
#define KbdGetStatus( k, i ) DKbdGetStatus( k )
#define KbdCharIn( k, x, y ) DKbdCharIn( k );
#define KbdPeek( k, x ) DKbdPeek( k );
#define VioGetCurPos( r, c, z ) DVioGetCurPos( r, c )
#define VioSetCurPos( r, c, z ) DVioSetCurPos( r, c )
#define VioWrtCharStr( c, l, r, d, z ) DVioWrtCharStr( c, l, r, d )
#define VioReadCharStr( c, l, r, d, z ) DVioReadCharStr( c, l, r, d )
#define VioWrtNChar( c, t, r, d, z ) DVioWrtNChar( c, t, r, d )
#define VioSetCurType( c, z ) DVioSetCurType( c )
#define VioGetCurType( c , z ) DVioGetCurType( c )
#define VioReadCellStr( b, l, r, c, z ) DVioReadCellStr( b, l, r, c );
#define VioWrtCellStr( b, l, r, c, z ) DVioWrtCellStr( b, l, r, c );
#define DosOpen( n, h, ac, s, at, fl, md, z ) DDosOpen( n, h )
#define DosFindFirst( s, h, a, b, l, c, z ) DDosFindFirst( s, a, b )
#define DosFindNext( h, b, l, c ) DDosFindNext( b )
#define DosQCurDisk( n, m ) DDosQCurDisk( n )
#define DosChDir( b, z ) DDosChDir( b )
#define DosAllocSeg( a,b,f ) DDosAllocSeg( a,b )
extern int      DosFreeEnv( void );
extern USHORT   DosRead( USHORT, char __far *, USHORT, USHORT PASPTR *);
extern USHORT   DosWrite( USHORT, char __far *, USHORT, USHORT PASPTR *);
extern USHORT   DDosAllocSeg( unsigned, int PASPTR * );
extern USHORT   DosGetEnv( USHORT PASPTR *, USHORT PASPTR * );
extern USHORT   DVioReadCellStr( char PASPTR *, USHORT PASPTR *, USHORT, USHORT );
extern USHORT   DDosOpen( char PASPTR *, USHORT PASPTR * );
extern USHORT   DVioWrtCharStr( char PASPTR *, USHORT, USHORT, USHORT );
extern USHORT   DVioSetCurType( CURSOR PASPTR * );
extern USHORT   DVioSetCurPos( USHORT, USHORT );
extern USHORT   DVioGetCurPos( USHORT PASPTR *, USHORT PASPTR * );
extern USHORT   DosClose( USHORT );
extern USHORT   DKbdCharIn( KBDCHAR PASPTR * );
extern USHORT   DVioGetCurType( CURSOR PASPTR * );
extern USHORT   DDosFindFirst( char PASPTR *, int, DIRINFO PASPTR * );
extern USHORT   DVioReadCharStr( char PASPTR *, USHORT PASPTR *, USHORT, USHORT );
extern USHORT   DVioWrtNChar( UCHAR PASPTR *, int, USHORT, USHORT );
extern USHORT   DosChgFilePtr( int, long, int, unsigned long PASPTR * );
extern USHORT   DDosFindNext( DIRINFO PASPTR * );
extern USHORT   DVioWrtCellStr( char PASPTR *, USHORT, USHORT, USHORT );
extern USHORT   DKbdGetStatus( KBDDESC PASPTR * );
extern USHORT   DDosChDir( char PASPTR *);
extern USHORT   DosQCurDir( int, char PASPTR *, int PASPTR * );
extern USHORT   DosSelectDisk( int );
extern USHORT   DDosQCurDisk( USHORT PASPTR *);

#endif

#define GLOBAL extern
#include "cmddata.h"

extern int      NonFileChar( char ch );
extern char     __far *GetEnv( char __far *name, int len );
extern char     __far *InitAlias( char __far *inname );
extern int      ExpandDirCommand( void );
extern int      LocateLeftWord( void );
extern int      LocateRightWord( void );
extern void     ToLastCmd( void );
extern void     ToFirstCmd( void );
extern void     BOL( void );
extern void     BackSpace( void );
extern void     Delete( void );
extern void     DeleteBOW( void );
extern void     DeleteEOW( void );
extern void     DoDirCommand( void );
extern void     DownScreen( void );
extern void     EOL( void );
extern void     EraseBOL( void );
extern void     EraseEOL( void );
extern void     EraseLine( void );
extern void     FlipInsertMode( void );
extern void     FlipScreenCursor( void );
extern void     InitSave( char __far *name );
extern void     InsertChar( void );
extern void     Left( void );
extern void     LeftScreen( void );
extern void     LeftWord( void );
extern void     ListAliases( void );
extern void     ListCommands( void );
extern void     LookForAlias( void );
extern void     MatchACommand( int (*advance)(char *), int (*retreat)(char *) );
extern void     FiniFile( void );
extern void     NextFile( void );
extern void     OverlayChar( void );
extern void     PFKey( void );
extern void     PrevFile( void );
extern void     PutChar( char ch );
extern void     PutNL( void );
extern void     PutString( char *str );
extern void     ReadScreen( int next_line );
extern void     RestoreLine( void );
extern void     RetrieveACommand( int (* advance)(char *) );
extern void     Right( void );
extern void     RightScreen( void );
extern void     RightWord( void );
extern void     SaveCmd( char *, unsigned );
extern void     SaveLine( void );
extern void     ScreenCursorOff( void );
extern void     SetCursorType( void );
extern void     UpScreen( void );
extern int      NextCmd( char * );
extern int      PrevCmd( char * );
extern int      DelCmd( char * );

extern int      WordSep( char ch );
extern char     *EatWhite( char *word );
extern void     RestorePrompt( char PASPTR *line );
extern void     SavePrompt( char PASPTR *line );
extern int      PutMore( void );
extern int      Equal( char __far *str1, char __far *str2, int len );

extern void     PutPad( char *str, int len );
extern int      ExpandDirCommand( void );
extern int      ReplaceAlias( char __far *alias, char *word, char *endword );
extern void     ZapLower( char *str );

extern void     SetCurPosWithScroll( int row, int col );

/* assembler interface declaration */
extern void     __far SaveSave( char __far *file );
extern void     __near InitRetrieve( char __far * );
extern int      __near StringIn( char __far *, LENGTH __far *, int, int );
#ifdef __DOS__
extern char     __far * __far AliasList;
#else
extern char     __far *AliasList;
#endif
