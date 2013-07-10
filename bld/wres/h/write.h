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


#ifndef WRESWRITE_INCLUDED
#define WRESWRITE_INCLUDED

#include "mresfmt.h"

extern int (*ConvToUnicode)( int, const char *, char *);

extern int ResWriteUint8( const uint_8 * newint, WResFileID handle );
extern int ResWriteUint16( const uint_16 * newint, WResFileID handle );
extern int ResWriteUint32( const uint_32 * newint, WResFileID handle );
extern int WResWriteLangRecord( const WResLangInfo *info, WResFileID handle );
extern int WResWriteResRecord( const WResResInfo * res, WResFileID fp );
extern int WResWriteTypeRecord( const WResTypeInfo * type, WResFileID fp );
extern int WResWriteWResID( const WResID * name, WResFileID fp );
extern int WResWriteWResIDName( const WResIDName * name, WResFileID fp );
extern int WResWriteWResIDNameUni( const WResIDName * name, uint_8 use_unicode, WResFileID handle );
extern int WResWriteHeaderRecord( const WResHeader * header, WResFileID handle );
extern int WResWriteExtHeader( const WResExtHeader * ext_head, WResFileID handle );
extern void MResFreeResourceHeader( MResResourceHeader * oldheader );
extern int ResWriteNameOrOrdinal( ResNameOrOrdinal * name, uint_8 use_unicode, WResFileID handle );
extern int ResWriteString( char * string, uint_8 use_unicode, WResFileID handle);
extern int ResWriteStringLen( char * string, uint_8 use_unicode, WResFileID handle, uint_16 len );
extern void WriteInitStatics( void );
extern int MResWriteResourceHeader( MResResourceHeader * currhead, WResFileID handle, char iswin32 );

#endif
