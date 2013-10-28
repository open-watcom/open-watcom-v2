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
#ifndef   DOS
    #define INCL_SUB
    #define INCL_DOSQUEUES
    #define INCL_DOSPROCESS
    #define INCL_DOSMISC
    #include <os2.h>
#endif

/* define DOS if compiling for dos */

/* OPTIONS start here */

#define PATH "PATH="
#ifdef   DOS
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

#ifdef DOS

#pragma aux pascal "*";
#define pasrtn int
#define PASPTR /**/
#define USHORT  unsigned short

enum { FALSE, TRUE };

#else

#pragma aux pascal "*" parm routine reverse [];
#define pasrtn int __far __pascal
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

#ifdef DOS

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
    CHAR        cFileName[ 260 ];
    CHAR        cAlternateFileName[ 16 ];
} WIN32_FIND_DATA;

#else

typedef struct _KBDKEYINFO KBDCHAR;

typedef struct _KBDINFO KBDDESC;

typedef struct _VIOCURSORINFO CURSOR;

typedef struct _FILEFINDBUF DIRINFO;

#endif

#ifdef DOS
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

extern char __far * FindAlias( char * word, char * endword, char __far * __far *start );

#ifdef DOS

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
extern pasrtn   DosRead( USHORT, char __far *, USHORT, USHORT PASPTR *);
extern pasrtn   DosWrite( USHORT, char __far *, USHORT, USHORT PASPTR *);
extern int      DDosAllocSeg( unsigned, int PASPTR * );
extern pasrtn   DosGetEnv( USHORT PASPTR *, USHORT PASPTR * );
extern int      DVioReadCellStr( char PASPTR *, USHORT PASPTR *, USHORT, USHORT );
extern int      DDosOpen( char PASPTR *, USHORT PASPTR * );
extern int      DVioWrtCharStr( char PASPTR *, USHORT, USHORT, USHORT );
extern int      DVioSetCurType( CURSOR PASPTR * );
extern int      DVioSetCurPos( USHORT, USHORT );
extern int      DVioGetCurPos( USHORT PASPTR *, USHORT PASPTR * );
extern int      DosClose( USHORT );
extern int      DKbdCharIn( KBDCHAR PASPTR * );
extern int      DVioGetCurType( CURSOR PASPTR * );
extern int      DDosFindFirst( char PASPTR *, int, DIRINFO PASPTR * );
extern int      DVioReadCharStr( char PASPTR *, USHORT PASPTR *, USHORT, USHORT );
extern int      DVioWrtNChar( UCHAR PASPTR *, int, USHORT, USHORT );
extern int      DosChgFilePtr( int, long, int, unsigned long PASPTR * );
extern int      DDosFindNext( DIRINFO PASPTR * );
extern int      DVioWrtCellStr( char PASPTR *, USHORT, USHORT, USHORT );
extern int      DKbdGetStatus( KBDDESC PASPTR * );
extern pasrtn   DDosChDir( char PASPTR *);
extern pasrtn   DosQCurDir( int, char PASPTR *, int PASPTR * );
extern pasrtn   DosSelectDisk( int );
extern pasrtn   DDosQCurDisk( USHORT PASPTR *);

#endif

#define GLOBAL extern
#include "cmddata.h"
