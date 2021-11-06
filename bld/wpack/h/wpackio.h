/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  I/O routines for wpack.
*
****************************************************************************/


extern int          IOStatus;

extern void         WriteMsg( const char *msg );
#if _WPACK
extern void         WriteLen( const char *msg, int len );
extern void         IndentLine( int amount );
extern void         WriteNumeric( const char *msg, unsigned long num );
#endif
extern int          QRead( int file, void *buffer, int amount );
extern int          QWrite( int file, void *buffer, int amount );
extern int          QOpenR( const char *filename );
extern int          NoErrOpen( const char *filename );
extern int          QOpenW( const char *filename );
#if _WPACK
extern int          QOpenM( const char *filename );
extern unsigned long QFileLen( int file );
extern unsigned long QGetDate( int handle );
#endif
extern void         QSeek( int file, long position, int seektype );
extern void         QClose( int file );
extern void         QSetDate( const char *fname, unsigned long stamp );
extern int          InitIO( void );
extern void         FiniIO( void );
extern int          BufSeek( unsigned long position );
#if _WPACK
extern void         CopyInfo( int dstfile, int srcfile, unsigned long len );
extern int          WriteSeek( unsigned long position );
#endif
extern byte         EncReadByte( void );
extern void         UnReadByte( byte value );
#if _WPACK
extern void         EncWriteByte( byte c );
#endif
extern byte         DecReadByte( void );
extern void         DecWriteByte( byte c );
#ifdef _M_IX86
#pragma aux DecReadByte __parm __nomemory __modify __nomemory
#pragma aux DecWriteByte __parm __nomemory __modify __nomemory
#endif
extern void         FlushWrite( void );
extern void         FlushRead( void );
#if _WPACK
extern void         SwitchBuffer( int handle, bool iswrite, void *buf );
extern void         RestoreBuffer( bool iswrite );
extern void         WriteFiller( unsigned amount );
extern unsigned_32  GetCRC( void );
#endif
extern void         ModifyCRC( unsigned long *value, byte data );
extern bool         CheckCRC( unsigned_32 value );
