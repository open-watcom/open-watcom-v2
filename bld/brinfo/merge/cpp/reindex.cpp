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


#include "wpch.hpp"
#include "reindex.hpp"
#include "cache.hpp"

// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions...
#pragma warning 549 9
static Pool UInt32Pair::_pairPool( U32PairPool, sizeof(UInt32Pair) );
#pragma warning 549 3


class PCHIndexRec : public HashTable<UInt32Pair> {
    public:
        PCHIndexRec( BRI_StringID fname );

        void            SetStart(uint_32 s) { _start = s; }
        uint_32         GetStart() { return _start; }
        void            SetStop(uint_32 s) { _stop = s; }
        uint_32         GetStop() { return _stop; }

        BRI_StringID    fileName() { return _fileName; }
    private:
        BRI_StringID    _fileName;
        uint_32         _start;
        uint_32         _stop;
};

PCHIndexRec::PCHIndexRec( BRI_StringID fname )
    : _fileName( fname )
    , HashTable<UInt32Pair>( 67 )       // a nice small prime number
{
    // empty
}


ReOrdering::ReOrdering()
    : HashTable<UInt32Pair>( 67 )       // a nice small prime number
{
    // Start the id's at 16 to save room for possible special values
    // ( 0 already means 'nil' for some types of browse records ) .

    _currentID = 0x10;
    _lastTopID = 0x10;
    _recordingPCH = NULL;
}


ReOrdering::~ReOrdering()
/***********************/
{
    PCHIndexRec *current;

    delete _recordingPCH;
    current = _storedPCH.First();
    while( current != NULL ){
        delete current;
        current = _storedPCH.Next();
    }
    _storedPCH.Clear();
    _usingPCH.Clear();
}


void ReOrdering::ChangeTo( uint_32 oldID, uint_32 newID )
/*******************************************************/
{
    UInt32Pair *        new_pair = new UInt32Pair( oldID, newID );

    Insert( new_pair );
    if( _recordingPCH != NULL ){
        _recordingPCH->Insert( new UInt32Pair(*new_pair) );
    }
}


uint_32 ReOrdering::Change( uint_32 oldID )
/*****************************************/
{
    UInt32Pair *        new_pair = new UInt32Pair( oldID, _currentID );

    Insert( new_pair );
    if( _recordingPCH != NULL ){
        _recordingPCH->Insert( new UInt32Pair(*new_pair) );
    }

    return _currentID++;
}


uint_32 ReOrdering::StealId()
/***************************/
{
    return _currentID++;
}


uint_32 ReOrdering::NewId( uint_32 oldID )
/****************************************/
{
    UInt32Pair          *pair = Lookup( oldID );
    PCHIndexRec         *current;

    if( pair != NULL ){
        return pair->newIndex;
    } else {
        current = _usingPCH.First();
        while( current != NULL ){
            pair = current->Lookup( oldID );
            if( pair != NULL ){
                return pair->newIndex;
            }
            current = _usingPCH.Next();
        }
        return 0;
    }
}


void ReOrdering::Flush()
/**********************/
{
    ClearAndDelete();
    _lastTopID = _currentID;
}


WBool ReOrdering::HaveSeenPCH( BRI_StringID fileName )
/****************************************************/
{
    WBool       result;
    PCHIndexRec *current;

    result = FALSE;
    current = _storedPCH.First();
    while( current != NULL ){
        if( current->fileName() == fileName ){
            result = TRUE;
            break;
        }
        current = _storedPCH.Next();
    }
    return result;
}


void ReOrdering::PCHRecord( BRI_StringID fileName )
/*************************************************/
{
    WAssert( _recordingPCH == NULL );

    _recordingPCH = new PCHIndexRec( fileName );
    _recordingPCH->SetStart( _currentID );
}


void ReOrdering::PCHStopRecording()
/*********************************/
{
    _recordingPCH->SetStop( _currentID );
    _storedPCH.Append( _recordingPCH );
    _usingPCH.Push( _recordingPCH );
    _recordingPCH = NULL;
}


void ReOrdering::PCHUse( BRI_StringID fileName )
/**********************************************/
{
    PCHIndexRec *current;

    current = _storedPCH.First();
    while( current != NULL ){
        if( current->fileName() == fileName ){
            break;
        }
        current = _storedPCH.Next();
    }
    WAssert( current != NULL );
    _usingPCH.Push( current );
}


void ReOrdering::PCHStopUsing()
/*****************************/
{
    _usingPCH.Clear();
}


WBool ReOrdering::SaveTo( CacheOutFile *cache, char const *name )
/***************************************************************/
{
    Hashable    *current;
    UInt32Pair  *currPair;
    PCHIndexRec *currIndex;
    int         i;

    cache->StartComponent( name );
    cache->AddDword( _currentID );
    cache->AddDword( _storedPCH.Count() );
    currIndex = _storedPCH.First();
    while( currIndex != NULL ){
        cache->AddDword( currIndex->fileName() );
        cache->AddDword( currIndex->GetStart() );
        cache->AddDword( currIndex->GetStop() );
        cache->AddDword( currIndex->Count() );
        for( i=0; i<currIndex->NumBuckets(); i++ ){
            current = _table[i];
            while( current != NULL ){
                currPair = (UInt32Pair *) current;
                cache->AddDword( currPair->index );
                cache->AddDword( currPair->newIndex );
                current = current->next;
            }
        }
        currIndex = _storedPCH.Next();
    }
    cache->EndComponent();

    return TRUE;
}


WBool ReOrdering::LoadFrom( CacheInFile *cache, char const *name )
/****************************************************************/
{
    WBool               result;
    UInt32Pair          *newPair;
    PCHIndexRec         *newIndex;
    int                 indexCount;
    int                 defnCount;
    BRI_StringID        fileName;
    uint_32             id;

    result = cache->OpenComponent( name );

    if( !result ){
        return result;
    }

    cache->ReadDword( &_currentID );
    _lastTopID = _currentID;
    cache->ReadDword( &indexCount );
    while( indexCount > 0 ){
        cache->ReadDword( &fileName );
        newIndex = new PCHIndexRec( fileName );
        cache->ReadDword( &id );
        newIndex->SetStart( id );
        cache->ReadDword( &id );
        newIndex->SetStop( id );
        cache->ReadDword( &defnCount );
        while( defnCount > 0 ){
            newPair = new UInt32Pair;
            cache->ReadDword( &newPair->index );
            cache->ReadDword( &newPair->newIndex );
            newIndex->Insert( newPair );
            defnCount--;
        }
        _storedPCH.Append( newIndex );
        indexCount--;
    }
    cache->CloseComponent();
    return result;
}


WBool ReOrdering::IsPCHIndex( uint_32 newID )
/*******************************************/
{
    WBool       result;
    PCHIndexRec *current;

    result = FALSE;
    current = _storedPCH.First();
    while( current != NULL ){
        if( newID >= current->GetStart() && newID < current->GetStop() ){
            result = TRUE;
            break;
        }
        current = _storedPCH.Next();
    }

    return result;
}
