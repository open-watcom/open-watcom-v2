/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  misc.c interfaces ( file search wildard function ).
*
****************************************************************************/


#ifndef __MISC_H
#define __MISC_H

#define GET_RECLEN()        MGET_LE_U16_UN(RecHdr + 1)
#define GET_RECCMD()        RecHdr[0]
#define IS_CMD32()          ((RecHdr[0] & 1) != 0)
#define GET_LEN(s)          ((unsigned char *)(s))[0]

typedef unsigned char   byte;

extern char         *NamePtr;
extern byte         NameLen;
extern byte         RecHdr[3];

extern char         *DoWildCard( char *base );
extern void         DoWildCardClose( void );

extern void         ReadRecInit( void );
extern void         ReadRecFini( void );
extern bool         IsDataToRead( void );
extern unsigned_8   GetByte( void );
extern unsigned_16  GetUInt( void );
extern unsigned_32  GetOffset( void );
extern unsigned_16  GetIndex( void );
extern char         *GetNameBuf( char *name );
extern void         ReadSkip( int );
extern int          ReadRec( FILE *fp );
extern void         RewindReadRec( void );
extern bool         AlignFile( FILE *fp, int page_len );
extern bool         WriteReadRec( FILE *fo );
extern byte         *GetReadPtr( void );

#endif
