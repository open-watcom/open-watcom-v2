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

int WResReadFixedResRecord( WResResInfo *newres, WResFileID handle );
int WResReadFixedResRecord1( WResResInfo1 *newres, WResFileID handle );
int WResReadFixedResRecord2( WResResInfo *newres, WResFileID handle );

int WResReadFixedTypeRecord( WResTypeInfo * newtype, WResFileID handle );
int WResReadFixedTypeRecord2( WResTypeInfo * newtype, WResFileID handle );

WResResInfo * WResReadResRecord( WResFileID handle );
WResTypeInfo * WResReadTypeRecord( WResFileID handle );
WResID * WResReadWResID( WResFileID handle );
int WResReadFixedWResID( WResID * name, WResFileID handle );
int WResReadExtraWResID( WResID * name, WResFileID handle );
WResIDName * WResReadWResIDName( WResFileID handle );
int WResReadHeaderRecord( WResHeader * header, WResFileID handle );
int WResReadExtHeader( WResExtHeader * head, WResFileID handle );
int WResIsWResFile( WResFileID handle );
extern ResTypeInfo WResFindResType( WResFileID handle );
MResResourceHeader * MResReadResourceHeader( WResFileID handle );
ResNameOrOrdinal * ResReadNameOrOrdinal( WResFileID handle );
ResNameOrOrdinal * ResRead32NameOrOrdinal( WResFileID handle );
extern char * ResReadString( WResFileID handle, int * strlen );
extern char * ResRead32String( WResFileID handle, int * strlen );
extern int ResReadUint32( uint_32 * newint, WResFileID handle );
extern int ResReadUint16( uint_16 * newint, WResFileID handle );
extern int ResReadUint8( uint_8 * newint, WResFileID handle );
extern long ResTell( WResFileID handle );
extern WResSeekReturn ResSeek( WResFileID handle, long offset, int origin );
extern int ResPadDWord( WResFileID handle );
extern M32ResResourceHeader *M32ResReadResourceHeader( WResFileID handle );

#endif
