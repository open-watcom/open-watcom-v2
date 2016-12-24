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

extern bool WResReadFixedResRecord( WResResInfo *newres, WResFileID fid );
extern bool WResReadFixedResRecord1( WResResInfo1 *newres, WResFileID fid );
extern bool WResReadFixedResRecord2( WResResInfo *newres, WResFileID fid );

extern bool WResReadFixedTypeRecord( WResTypeInfo *newtype, WResFileID fid );
extern bool WResReadFixedTypeRecord1or2( WResTypeInfo *newtype, WResFileID fid );

extern WResResInfo      *WResReadResRecord( WResFileID fid );
extern WResTypeInfo     *WResReadTypeRecord( WResFileID fid );
extern WResID           *WResReadWResID( WResFileID fid );
extern bool             WResReadFixedWResID( WResID *name, WResFileID fid );
extern bool             WResReadExtraWResID( WResID *name, WResFileID fid );
extern WResIDName       *WResReadWResIDName( WResFileID fid );
extern bool             WResReadHeaderRecord( WResHeader *header, WResFileID fid );
extern bool             WResReadExtHeader( WResExtHeader *head, WResFileID fid );
extern bool             WResIsWResFile( WResFileID fid );
extern ResTypeInfo      WResFindResType( WResFileID fid );
extern MResResourceHeader *MResReadResourceHeader( WResFileID fid );
extern ResNameOrOrdinal *ResReadNameOrOrdinal( WResFileID fid );
extern ResNameOrOrdinal *ResRead32NameOrOrdinal( WResFileID fid );
extern char             *ResReadString( WResFileID fid, size_t *strlen );
extern char             *ResRead32String( WResFileID fid, size_t *strlen );
extern bool             ResReadUint32( uint_32 *newint, WResFileID fid );
extern bool             ResReadUint16( uint_16 *newint, WResFileID fid );
extern bool             ResReadUint8( uint_8 *newint, WResFileID fid );
extern WResFileOffset   ResTell( WResFileID fid );
extern WResFileOffset   ResSeek( WResFileID fid, WResFileOffset offset, int origin );
extern bool             ResReadPadDWord( WResFileID fid );
extern M32ResResourceHeader *M32ResReadResourceHeader( WResFileID fid );

#endif
