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


#include <wclistit.h>
#include "common.hpp"
#include "cverror.hpp"
#include "typearray.hpp"

void GlobalHashContainer::Insert( LFTypeRecord* t )
/*************************************************/
{
    if ( ++_entries / _bucketSize > TABLE_LOAD ) {
        Rehash();
    }
    _buckets[HFn(t->Variant())%_bucketSize].append(t);
}

void GlobalHashContainer::Rehash()
/********************************/
{
    uint newSize = _bucketSize << 1;
    WCPtrSList<LFTypeRecord>* newBucket = new WCPtrSList<LFTypeRecord> [newSize];
    WCPtrSListIter<LFTypeRecord> iter;
    for ( uint i = 0; i < _bucketSize; i++ ) {
        iter.reset(_buckets[i]);
        while ( ++iter ) {
            newBucket[HFn(iter.current()->Variant())%newSize].append(iter.current());
        }
    }
    delete [] _buckets;
    _bucketSize = newSize;
    _buckets = newBucket;
}

unsigned_32 GlobalHashContainer::HFn( VariantString& var )
/********************************************************/
{
    char* string = var._string;
    uint len = var._strLen;
    unsigned_32 h = 0;
    unsigned_32 g;

    while ( len-- ) {
        h = ( h << 4 ) + *string++;
        g = h & 0xf0000000;
        if ( g ) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

GlobalTypeArray::GlobalTypeArray()
/********************************/
{
    _mapTypeArray[0] = NULL;
    for ( unsigned_8 i = LF_MODIFIER; i <= LF_ARRAY; i++ ) {
        _mapTypeArray[i] = new GlobalListContainer;
    }
    for ( i = LF_CLASS; i <= LF_ENUM; i++ ) {
        _mapTypeArray[i] = new GlobalHashContainer;
    }
    for ( i = LF_PROCEDURE; i <= LF_OEM; i++ ) {
        _mapTypeArray[i] = new GlobalListContainer;
    }
}

void GlobalTypeArray::DerivedInsert( LFTypeRecord* t )
/****************************************************/
{
    if ( t -> Index() <= LF_OEM ) {
        _mapTypeArray[t->Index()]->Insert(t);
        return;
    }
}

WCPtrSList<LFTypeRecord>& GlobalTypeArray::Find( LFTypeRecord* t )
/****************************************************************/
{
    if ( t -> Index() <= LF_OEM ) {
        return _mapTypeArray[t->Index()]->Find(t);
    }
    throw InternalError("GlobalTypeArray::Find gets call with an indirect index");
}

void LocalTypeArray::DerivedInsert( LFTypeRecord* t )
/***************************************************/
{
    if ( t -> Index() >= LF_CLASS && t -> Index() <= LF_ENUM ) {
        _hashRecords.append(t);
        return;
    }
    _otherRecords.append(t);
}
