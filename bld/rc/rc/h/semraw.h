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
* Description:  Semantic actions for processing raw resource data.
*
****************************************************************************/


#ifndef SEMRAW_INCLUDED
#define SEMRAW_INCLUDED

#include "watcom.h"
#include "rctypes.h"

#define MAX_DATA_NODES     100

typedef struct RawDataItem {
    uint_8      IsString;
    uint_8      LongItem;
    uint_16     StrLen;
    uint_8      TmpStr;
    uint_8      WriteNull;
    union {
        char    *String;
        uint_32  Num;
    } Item;
} RawDataItem;

typedef struct DataElemList {
    struct DataElemList   *next;
    RawDataItem            data[ MAX_DATA_NODES ];
    uint_16                count;
} DataElemList;

extern void SemWriteRawDataItem( RawDataItem item );
extern RcStatus SemCopyDataUntilEOF( long offset, int handle, void *buff,
                int buffsize, int *err_code );
extern ResLocation SemCopyRawFile( char *filename );
extern ResLocation SemFlushDataElemList( DataElemList *head, char call_startend );
extern DataElemList *SemAppendDataElem( DataElemList *head, RawDataItem node );
extern DataElemList *SemNewDataElemList( RawDataItem node );
extern DataElemList *SemNewDataElemListNode( void );
extern void SemFreeDataElemList( DataElemList *head );

#endif
