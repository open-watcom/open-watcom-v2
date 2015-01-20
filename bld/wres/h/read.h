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

typedef enum ResTypeInfo {
    RT_WATCOM,
    RT_WIN16,
    RT_WIN32
} ResTypeInfo;

extern bool WResReadFixedResRecord( WResResInfo *newres, WResFileID handle );
extern bool WResReadFixedResRecord1( WResResInfo1 *newres, WResFileID handle );
extern bool WResReadFixedResRecord2( WResResInfo *newres, WResFileID handle );

extern bool WResReadFixedTypeRecord( WResTypeInfo * newtype, WResFileID handle );
extern bool WResReadFixedTypeRecord2( WResTypeInfo * newtype, WResFileID handle );

extern WResResInfo      *WResReadResRecord( WResFileID handle );
extern WResTypeInfo     *WResReadTypeRecord( WResFileID handle );
extern WResID           *WResReadWResID( WResFileID handle );
extern bool             WResReadFixedWResID( WResID * name, WResFileID handle );
extern bool             WResReadExtraWResID( WResID * name, WResFileID handle );
extern WResIDName       *WResReadWResIDName( WResFileID handle );
extern bool             WResReadHeaderRecord( WResHeader * header, WResFileID handle );
extern bool             WResReadExtHeader( WResExtHeader * head, WResFileID handle );
extern bool             WResIsWResFile( WResFileID handle );
extern ResTypeInfo      WResFindResType( WResFileID handle );
extern MResResourceHeader *MResReadResourceHeader( WResFileID handle );
extern ResNameOrOrdinal *ResReadNameOrOrdinal( WResFileID handle );
extern ResNameOrOrdinal *ResRead32NameOrOrdinal( WResFileID handle );
extern char             *ResReadString( WResFileID handle, int * strlen );
extern char             *ResRead32String( WResFileID handle, int * strlen );
extern bool             ResReadUint32( uint_32 * newint, WResFileID handle );
extern bool             ResReadUint16( uint_16 * newint, WResFileID handle );
extern bool             ResReadUint8( uint_8 * newint, WResFileID handle );
extern WResFileOffset   ResTell( WResFileID handle );
extern WResFileOffset   ResSeek( WResFileID handle, WResFileOffset offset, int origin );
extern bool             ResPadDWord( WResFileID handle );
extern M32ResResourceHeader *M32ResReadResourceHeader( WResFileID handle );

#endif
