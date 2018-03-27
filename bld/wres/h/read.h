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


#ifndef WRESREAD_INCLUDED
#define WRESREAD_INCLUDED

#include "mresfmt.h"


#define UNI2ASCII(x)    ((char)((x) & 0xFF))

typedef enum ResTypeInfo {
    RT_WATCOM,
    RT_WIN16,
    RT_WIN32
} ResTypeInfo;

extern bool WResReadFixedResRecord( WResResInfo *newres, FILE *fp );
extern bool WResReadFixedResRecord1( WResResInfo1 *newres, FILE *fp );
extern bool WResReadFixedResRecord2( WResResInfo *newres, FILE *fp );

extern bool WResReadFixedTypeRecord( WResTypeInfo *newtype, FILE *fp );
extern bool WResReadFixedTypeRecord1or2( WResTypeInfo *newtype, FILE *fp );

extern WResResInfo          *WResReadResRecord( FILE *fp );
extern WResTypeInfo         *WResReadTypeRecord( FILE *fp );
extern WResID               *WResReadWResID( FILE *fp );
extern bool                 WResReadFixedWResID( WResID *name, FILE *fp );
extern bool                 WResReadExtraWResID( WResID *name, FILE *fp );
extern WResIDName           *WResReadWResIDName( FILE *fp );
extern bool                 WResReadHeaderRecord( WResHeader *header, FILE *fp );
extern bool                 WResReadExtHeader( WResExtHeader *head, FILE *fp );
extern bool                 WResIsWResFile( FILE *fp );
extern ResTypeInfo          WResFindResType( FILE *fp );
extern MResResourceHeader   *MResReadResourceHeader( FILE *fp );
extern ResNameOrOrdinal     *ResReadNameOrOrdinal( FILE *fp );
extern ResNameOrOrdinal     *ResRead32NameOrOrdinal( FILE *fp );
extern char                 *ResReadString( FILE *fp, size_t *strlen );
extern char                 *ResRead32String( FILE *fp, size_t *strlen );
extern bool                 ResReadUint32( uint_32 *newint, FILE *fp );
extern bool                 ResReadUint16( uint_16 *newint, FILE *fp );
extern bool                 ResReadUint8( uint_8 *newint, FILE *fp );
extern long                 ResTell( FILE *fp );
extern bool                 ResSeek( FILE *fp, long offset, int origin );
extern bool                 ResReadPadDWord( FILE *fp );
extern M32ResResourceHeader *M32ResReadResourceHeader( FILE *fp );

#endif
