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


#define FLAGS_16SYM     0
#define FLAGS_32SYM     1
#define FLAGS_ALPHA     2

typedef struct {
WORD ppNextMap;         /* *16 = pointer to next map */
BYTE bFlags;            /* see FLAGS */
BYTE bResereved1;
WORD ppSegEntry;        /* offset? to entry segment */
WORD cConsts;           /* number of constants */
WORD pConstDef;         /* offset from file start to constant definitions */
WORD cSegs;             /* number of segments */
WORD ppSegDef;          /* *16 = pointer to first segment definition */
BYTE cbMaxSym;          /* length of longest symbol in this module */
BYTE cbModName;         /* length of module name */
char achModName[1];     /* module name (not null terminated) */
} MAPDEF;
typedef MAPDEF far *LPMAPDEF;

typedef struct {
WORD ppNextSeg;         /* *16 = offset to next segment */
WORD cSymbols;          /* number of symbols in list */
WORD pSymDef;           /* offset from start of segdef to array of pointers */
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
WORD wReserved4;
BYTE bFlags;            /* see FLAGS */
BYTE bReserved1;
WORD pLineDef;          /* offset to line number record */
BYTE bReserved2;
BYTE bReserved3;
BYTE cbSegName;         /* length of segment name */
char achSegName[1];     /* segment name (not null terminated) */
} SEGDEF;
typedef SEGDEF far *LPSEGDEF;

typedef struct {
WORD wSymVal;           /* symbol address or constant value */
BYTE cbSymName;         /* length of symbol name */
char achSymName[1];     /* symbol name field */
} SYMDEF;
typedef SYMDEF CONSTDEF;
typedef SYMDEF far *LPSYMDEF;

typedef struct {
DWORD dwSymVal;         /* symbol address or constant value */
BYTE cbSymName;         /* length of symbol name */
char achSymName[1];     /* symbol name field */
} SYMDEF32;
typedef SYMDEF32 far *LPSYMDEF32;

typedef struct {
WORD ppNextLine;        /* *16 = next linedef */
WORD wReserved1;
WORD pLines;            /* offset from beginning of current LINEDEF to line-info */
WORD wReserved2;
int  cLines;            /* line number count */
BYTE cbFileName;        /* length of source file name */
char achFileName[1];    /* source file name (not null terminated) */
} LINEDEF;

typedef struct {
WORD wCodeOffset;       /* offset in segment */
DWORD dwFileOffset;     /* source offset */
} LINEINF;
