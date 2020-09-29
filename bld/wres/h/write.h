/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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

typedef size_t ConvToUnicode_fn( size_t, const char *, char * );
typedef size_t ConvToMultiByte_fn( size_t, const char *, char * );

extern ConvToUnicode_fn     *ConvToUnicode;
extern ConvToMultiByte_fn   *ConvToMultiByte;

extern bool ResWriteUint8( uint_8 newint, FILE *fp );
extern bool ResWriteUint16( uint_16 newint, FILE *fp );
extern bool ResWriteUint32( uint_32 newint, FILE *fp );
extern bool ResWritePadDWord( FILE *fp );
extern bool WResWriteLangRecord( const WResLangInfo *info, FILE *fp );
extern bool WResWriteResRecord( const WResResInfo *res, FILE *fp );
extern bool WResWriteTypeRecord( const WResTypeInfo *type, FILE *fp );
extern bool WResWriteWResID( const WResID *name, FILE *fp );
extern bool WResWriteWResIDName( const WResIDName *name, bool use_unicode, FILE *fp );
extern bool WResWriteHeaderRecord( const WResHeader *header, FILE *fp );
extern bool WResWriteExtHeader( const WResExtHeader *ext_head, FILE *fp );
extern void MResFreeResourceHeader( MResResourceHeader *oldheader );
extern bool ResWriteNameOrOrdinal( ResNameOrOrdinal *name, bool use_unicode, FILE *fp );
extern bool ResWriteString( const char *string, bool use_unicode, FILE *fp );
extern bool ResWriteStringLen( const char *string, bool use_unicode, FILE *fp, size_t len );
extern void WriteInitStatics( void );
extern bool MResWriteResourceHeader( MResResourceHeader *currhead, FILE *fp, bool iswin32 );

#endif
