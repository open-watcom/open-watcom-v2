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


#ifndef EXERES_INCLUDED
#define EXERES_INCLUDED

#include "exeos2.h"
#include "rcstr.h"
#include "wresall.h"

typedef struct FullResourceRecord {
    struct FullResourceRecord * Next;
    struct FullResourceRecord * Prev;
    resource_record             Info;
} FullResourceRecord;

typedef struct FullTypeRecord {
    struct FullTypeRecord * Next;
    struct FullTypeRecord * Prev;
    FullResourceRecord *    Head;
    FullResourceRecord *    Tail;
    resource_type_record    Info;
} FullTypeRecord;

typedef struct ExeResDir {
    uint_16             ResShiftCount;
    uint_16             NumTypes;
    uint_16             NumResources;
    uint_16             TableSize;
    FullTypeRecord *    Head;
    FullTypeRecord *    Tail;
} ExeResDir;

typedef struct ResTable {
    ExeResDir   Dir;
    StringBlock Str;
} ResTable;


extern void InitResTable( void );
extern uint_32 ComputeResourceSize( WResDir dir );
extern int CopyResources( uint_16 sect2mask, uint_16 sect2bits, bool sect2 );
extern RcStatus WriteResTable( int handle, ResTable *restab, int *err_code );

#endif
