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
* Description:  Header file to set up internal trap debugging code
*
****************************************************************************/


#ifdef DEBUG_TRAP

#include <watcom.h>
#include <string.h>

#define DBG_STDOUT_H                    1
#define DBG_PAGESIZE                    24      /* counting from 0 */
#define DBG_SHIFT_WIDTH                 3
#define DBG_UNKNOWN_REQ                 "UNKNOWN REQUEST"
#define DBG_ERRSTR                      " **** "
#define DBG_ENTER_MSG                   "ENTERING "
#define DBG_EXIT_MSG                    "EXITING "
#define DBG_PAUSE_MSG                   "Press any key to continue ..."
#define DBG_CR                          "\r"
#define DBG_LF                          "\n"
#define DBG_BLANK                       0x20
#define DBG_BELL                        0x07

#ifdef __LINUX__
#define _DBG_KeyWait()

#include <unistd.h>

extern void _DBG_DumpMultChars( uint_8 ch, uint_32 count, uint_32 fhandle );
#pragma aux _DBG_DumpMultChars =                                        \
    "push   ebp"                                                        \
    "mov    ebp, esp"                                                   \
    "sub    esp, edx"           /* make space for edx chars */          \
    "mov    edi, esp"                                                   \
    "mov    ecx, edx"           /* count in ecx */                      \
    "rep    stosb"              /* replicate chars in buffer */         \
    "mov    ecx, esp"           /* now ecx points to string */          \
    "mov    eax,4"                                                      \
    "int    0x80"                                                       \
    "mov    esp, ebp"                                                   \
    "pop    ebp"                                                        \
    parm [ al ] [ edx ] [ ebx ]                                         \
    modify [eax ecx esi edi];

extern void _DBG_DumpChar( uint_8 ch, uint_32 fhandle );
#pragma aux _DBG_DumpChar =                                             \
    "push   eax"                                                        \
    "mov    ecx, esp"           /* now ecx points to char */            \
    "mov    edx, 1"             /* number of bytes to write */          \
    "mov    eax,4"                                                      \
    "int    0x80"                                                       \
    "pop    eax"                                                        \
    parm [ al ] [ ebx ];

extern uint_8 _DBG_HexChar( uint_8 digit );
#pragma aux _DBG_HexChar =                                              \
    "and    al, 0fh"      /* the digit is in the low 4 bits */          \
    "cmp    al, 09h"                                                    \
    "jg     L2"                                                         \
    "add    al, '0'"                                                    \
    "jmp    L3"                                                         \
    "L2:"                                                               \
    "sub    al, 0ah"                                                    \
    "add    al, 'a'"                                                    \
    "L3:"                                                               \
    parm [ al ];

extern void _DBG_DumpStr( const char *str, uint_32 len, uint_32 fhandle );
#pragma aux _DBG_DumpStr =                                              \
    "mov    eax,4"                                                      \
    "int    0x80"                                                       \
    parm [ecx] [edx] [ebx];

#else

extern void _DBG_KeyWait( void );
#pragma aux _DBG_KeyWait =                                              \
    "mov        ah, 08h ",      /* read char, no echo, check ^C */      \
    "int        21h     "                                               \
    modify exact [ ax ];

extern void _DBG_DumpMultChars( uint_8 ch, uint_16 count, uint_16 fhandle );
#pragma aux _DBG_DumpMultChars =                                        \
    "push       ds      ",      /* don't modify ds */                   \
    "push       es      ",                                              \
    "push       bp      ",                                              \
    "mov        bp, sp  ",                                              \
    "sub        sp, cx  ",      /* make space for bx chars */           \
    "mov        di, sp  ",                                              \
    "mov        dx, ss  ",                                              \
    "mov        es, dx  ",      /* now es:di points to temp buffer */   \
    "mov        ds, dx  ",      /* ds needs dx, for later */            \
    "mov        dx, cx  ",      /* save count in dx */                  \
    "rep        stosb   ",      /* now ss:sp points to */               \
    "mov        cx, dx  ",      /* now cx holds count */                \
    "mov        dx, sp  ",      /* now ds:dx points to string */        \
    "mov        ah, 40h ",      /* write bytes to file w/handle */      \
    "int        21h     ",                                              \
    "mov        sp, bp  ",                                              \
    "pop        bp      ",                                              \
    "pop        es      ",                                              \
    "pop        ds      "                                               \
    parm [ al ] [ cx ] [ bx ]                                           \
    modify exact [ ax cx dx di ];

extern void _DBG_DumpChar( uint_8 ch, uint_16 fhandle );
#pragma aux _DBG_DumpChar =                                             \
    "push       ds      ",      /* don't modify ds */                   \
    "push       ax      ",                                              \
    "mov        ax, ss  ",                                              \
    "mov        ds, ax  ",                                              \
    "mov        dx, sp  ",      /* now ds:dx points to char */          \
    "mov        cx, 1   ",      /* number of bytes to write */          \
    "mov        ah, 40h ",      /* write bytes to file w/handle */      \
    "int        21h     ",                                              \
    "pop        ax      ",                                              \
    "pop        ds      "                                               \
    parm [ al ] [ bx ]                                                  \
    modify exact [ cx dx ];

extern uint_8 _DBG_HexChar( uint_8 digit );
#pragma aux _DBG_HexChar =                                              \
    "and        al, 0fh ",      /* the digit is in the low 4 bits */    \
    "cmp        al, 09h ",                                              \
    "jg         L2      ",                                              \
    "add        al, '0' ",                                              \
    "jmp        L3      ",                                              \
    "L2:                ",                                              \
    "sub        al, 0ah ",                                              \
    "add        al, 'a' ",                                              \
    "L3:                "                                               \
    parm [ al ]                                                         \
    modify exact [ al ];

extern void _DBG_DumpStr( const char __far *str, uint_16 len, uint_16 fhandle );
#pragma aux _DBG_DumpStr =                                              \
    "push       ds      ",      /* don't modify ds */                   \
    "xchg       ax, dx  ",      /* now ax=seg str, dx=off str */        \
    "mov        ds, ax  ",      /* now ds:dx=str, cx=len, bx=handle */  \
    "mov        ah, 40h ",      /* write bytes to file with handle */   \
    "int        21h     ",                                              \
    "pop        ds      "                                               \
    parm [ ax dx ] [ cx ] [ bx ]                                        \
    modify exact [ ax dx ];
#endif


#define _DBG_Request( n )       ( (access_req)(n) >= REQ__LAST ?        \
                                  DBG_UNKNOWN_REQ :                     \
                                  DBG_ReqStrings[ (access_req)(n) ] )
#define _DBG_Space()            _DBG_DumpChar( DBG_BLANK, DBG_STDOUT_H );
#define _DBG_RingBell()         _DBG_DumpChar( DBG_BELL, DBG_STDOUT_H );
#define _DBG_DumpHexChar( n )   _DBG_DumpChar( _DBG_HexChar( n ),       \
                                               DBG_STDOUT_H );
#define _DBG_Write8( n )        { _DBG_DumpHexChar( (uint_8)(n) >> 4 ); \
                                  _DBG_DumpHexChar( (uint_8)(n) ); }
#define _DBG_Write16( n )       { _DBG_Write8( (uint_16)(n) >> 8 );     \
                                  _DBG_Write8( (uint_8)(n) ); }
#define _DBG_Write32( n )       { _DBG_Write16( (uint_32)(n) >> 16 );   \
                                  _DBG_Write16( (uint_16)(n) ); }
#define _DBG_WriteConstStr( s ) _DBG_DumpStr( s, sizeof( s ) - 1, DBG_STDOUT_H );
#define _DBG_DumpBytes( p, n )  { uint_16       j;                      \
                                  for( j = 0; j < n; ++j ) {            \
                                      _DBG_Write8( *((uint_8 *)(p) + j) ); \
                                      _DBG_Space();                     \
                                  }                                     \
                                  _DBG_NewLine();                       \
                                }
#define _DBG_Tab( n )           _DBG_DumpMultChars( DBG_BLANK, n, DBG_STDOUT_H );
#define _DBG_NewLine()          { _DBG_WriteConstStr( DBG_CR DBG_LF );  \
                                    ++DBG_Lines; }
#define _DBG_ChkNewLn()         { _DBG_NewLine();                       \
                                  if( DBG_Lines >= DBG_PAGESIZE ) {     \
                                      _DBG_WriteConstStr( DBG_PAUSE_MSG ); \
                                      _DBG_KeyWait();                   \
                                      _DBG_NewLine();                   \
                                      DBG_Lines = 0;                    \
                                  }                                     \
                                }
#define _DBG_NoTabWrite( s )    _DBG_DumpStr( s, strlen( s ), DBG_STDOUT_H );
#define _DBG_Write( s )         { _DBG_Tab( DBG_Indent );               \
                                  _DBG_NoTabWrite( s ); }
#define _DBG_Writeln( s )       { _DBG_Write( s ); _DBG_NewLine(); }
#define _DBG_NoTabWriteln( s )  { _DBG_NoTabWrite( s ); _DBG_NewLine(); }
#define _DBG_EnterFunc( s )     { _DBG_Tab( DBG_Indent );               \
                                  _DBG_WriteConstStr( DBG_ENTER_MSG );  \
                                  _DBG_NoTabWriteln( s );               \
                                  DBG_Indent += DBG_SHIFT_WIDTH; }
#define _DBG_ExitFunc( s )      { DBG_Indent -= DBG_SHIFT_WIDTH;        \
                                  _DBG_Tab( DBG_Indent );               \
                                  _DBG_WriteConstStr( DBG_EXIT_MSG );   \
                                  _DBG_NoTabWriteln( s ); }
#define _DBG_WriteErr( s )      { _DBG_WriteConstStr( DBG_ERRSTR );     \
                                  _DBG_NoTabWrite( s );                 \
                                  _DBG_WriteConstStr( DBG_ERRSTR );     \
                                  _DBG_NewLine();                       \
                                  _DBG_RingBell();                      \
                                }


extern uint_8                   DBG_Indent;
extern uint_8                   DBG_Lines;
extern char                     *DBG_ReqStrings[];


#else


#define _DBG_Request( n )

#define _DBG_KeyWait()
#define _DBG_Space()
#define _DBG_Tab( n )
#define _DBG_RingBell()

#define _DBG_DumpHexChar( n )
#define _DBG_Write8( n )
#define _DBG_Write16( n )
#define _DBG_Write32( n )
#define _DBG_DumpBytes( p, n )

#define _DBG_NewLine()
#define _DBG_ChkNewLn()

#define _DBG_NoTabWrite( s )
#define _DBG_Write( s )
#define _DBG_NoTabWriteln( s )
#define _DBG_Writeln( s )

#define _DBG_EnterFunc( s )
#define _DBG_ExitFunc( s )
#define _DBG_WriteErr( s )


#endif  /* #ifdef DEBUG_TRAP */
