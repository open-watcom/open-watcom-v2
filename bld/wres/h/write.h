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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef WRESWRITE_INCLUDED
#define WRESWRITE_INCLUDED

#include "mresfmt.h"

typedef size_t ConvToUnicode_fn( const char *, size_t, char *, size_t );
typedef size_t ConvToMultiByte_fn( const char *, size_t, char *, size_t );

extern ConvToUnicode_fn     *ConvToUnicode;
extern ConvToMultiByte_fn   *ConvToMultiByte;

extern bool ResWriteUint8( FILE *fp, uint_8 value );
extern bool ResWriteUint16( FILE *fp, uint_16 value );
extern bool ResWriteUint32( FILE *fp, uint_32 value );
extern bool ResWritePadDWord( FILE *fp );
extern bool WResWriteLangRecord( const WResLangInfo *info, FILE *fp );
extern bool WResWriteResRecord( const WResResInfo *res, FILE *fp );
extern bool WResWriteTypeRecord( const WResTypeInfo *type, FILE *fp );
extern bool WResWriteWResID( const WResID *id, FILE *fp );
extern bool WResWriteWResIDName( const WResIDName *name_id, FILE *fp );
extern bool WResWriteWResIDNameString( const WResIDName *name_id, bool use_unicode, FILE *fp );
extern bool WResWriteHeader( const WResHeader *header, FILE *fp );
extern bool WResWriteExtHeader( const WResExtHeader *extheader, FILE *fp );
extern bool ResWriteNameOrOrdinal( ResNameOrOrdinal *name, bool use_unicode, FILE *fp );
extern bool ResWriteString( const char *str, bool use_unicode, FILE *fp );
extern bool ResWriteStringLen( const char *str, size_t len, bool use_unicode, bool with_len, FILE *fp );
extern void WriteInitStatics( void );
extern void MResFreeResourceHeader( MResResourceHeader *msheader );
extern bool MResWriteResourceHeader( MResResourceHeader *msheader, bool iswin32, FILE *fp );

#endif
