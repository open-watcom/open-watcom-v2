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
#include "decl.hpp"
#include "macro.hpp"
#include "handle.hpp"
#include "avltree.hpp"
#include "cache.hpp"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

extern Browser  *CurrBrowser;

#define LIST_BLOCK      0x100
#define BRM_NUM_IDS     0x10

struct StringIDs {
    StringIDs();
    StringIDs( int startSize );
    ~StringIDs();

    void                DoubleSize();

    BRI_StringID        *strings;
    int                 maxEntries;
    int                 numEntries;
};

struct FileGuardInfo {
    FileGuardInfo();
    FileGuardInfo( BRI_StringID fname );
    ~FileGuardInfo();

    FileGuardInfo               *next;
    BRI_StringID                filenameID;
    StringIDs                   defined;
    StringIDs                   undefed;
    AvlTree<DependValRec>       values;
    int                         numValues;
};


int compareStringIDs( const void *op1, const void *op2 )
/******************************************************/
{
    return *((BRI_StringID *) op2) - *((BRI_StringID *) op1);
}


StringIDs::StringIDs()
/********************/
{
    strings = NULL;
    maxEntries = 0;
    numEntries = 0;
}


StringIDs::StringIDs( int startSize )
/***********************************/
{
    strings = new BRI_StringID[startSize];
    maxEntries = startSize;
    numEntries = 0;
}


StringIDs::~StringIDs()
/*********************/
{
    delete[] strings;
}


inline void StringIDs::DoubleSize()
/*********************************/
{
    BRI_StringID        *newNames;

    newNames = new BRI_StringID[ 2*maxEntries ];
    memcpy( newNames, strings,
            maxEntries * sizeof(BRI_StringID) );
    delete[] strings;
    strings = newNames;
    maxEntries *= 2;
}


FileGuardInfo::FileGuardInfo()
/****************************/
{
    next = NULL;
    filenameID = (BRI_StringID) 0;
    numValues = 0;
}


FileGuardInfo::FileGuardInfo(BRI_StringID fname)
: defined( BRM_NUM_IDS )
, undefed( BRM_NUM_IDS )
/**********************************************/
{
    next = NULL;
    filenameID = fname;
    numValues = 0;
}


FileGuardInfo::~FileGuardInfo()
/*****************************/
{
    DependValRec        *val;

    val = values.First();
    while( val != NULL ){
        if( val->defn != NULL ){
            delete[] val->defn;
        }
        delete val;
        val = values.Next();
    }
}


DependList::DependList()
/**********************/
{
    _fileInfo = new AvlTree<FileGuardInfo>;
    _count = 0;
    _state = USE;
    _temp = NULL;
}


DependList::~DependList()
/***********************/
{
    FileGuardInfo       *current;
    FileGuardInfo       *prev;

    if( _temp != NULL ){
        delete _temp;
    }
    current = _fileInfo->First();
    while( current != NULL ){
        do {
            prev = current;
            current = current->next;
            delete prev;
        } while( current != NULL );
        current = _fileInfo->Next();
    }
    delete _fileInfo;
}


void DependList::StartFile( BRI_StringID filename )
/*************************************************/
{
    if( _templateDepth > 0 ){
        return;
    }

    if( _state == USE ){
        if( _temp != NULL ){
            delete _temp;
        }
        _temp = new FileGuardInfo(filename);
    } else {
        _ignoreDepth += 1;
    }

    return;
}


void DependList::AddDepDefined( BRI_StringID macroName )
/******************************************************/
{
    if( _templateDepth > 0 ){
        return;
    }

    WAssert( _temp != NULL );
    if( _temp->defined.numEntries == _temp->defined.maxEntries ){
        _temp->defined.DoubleSize();
    }
    _temp->defined.strings[_temp->defined.numEntries++] = macroName;

    return;
}


void DependList::AddDepUndefed( BRI_StringID macroName )
/******************************************************/
{
    if( _templateDepth > 0 ){
        return;
    }

    WAssert( _temp != NULL );
    if( _temp->undefed.numEntries == _temp->undefed.maxEntries ){
        _temp->undefed.DoubleSize();
    }
    _temp->undefed.strings[_temp->undefed.numEntries++] = macroName;
    return;
}


void DependList::AddDepMacValue( DependValRec *value )
/****************************************************/
{
    DependValRec        *previous;

    if( _templateDepth > 0 ){
        return;
    }

    WAssert( _temp != NULL );
    previous = _temp->values.Find( value->macroNameID );
    if( previous == NULL ){
        _temp->values.Insert( value->macroNameID, value );
        _temp->numValues++;
    } else {
        delete value;
    }

    return;
}


WBool DependList::IncludeFile()
/***************************/
{
    WBool               result;
    FileGuardInfo       *fileInfo, *headInfo;
    DependValRec        *value, *other;

    if( _templateDepth > 0 ){
        return TRUE;
    }

    WAssert( _temp != NULL );
    qsort( _temp->defined.strings, _temp->defined.numEntries,
           sizeof(BRI_StringID), &::compareStringIDs );
    qsort( _temp->undefed.strings, _temp->undefed.numEntries,
           sizeof(BRI_StringID), &::compareStringIDs );
    result = TRUE;
    headInfo = _fileInfo->Find( _temp->filenameID );
    fileInfo = headInfo;
    for( ; fileInfo != NULL && result; fileInfo = fileInfo->next ){
        if( _temp->defined.numEntries != fileInfo->defined.numEntries ){
            continue;
        }
        if( _temp->undefed.numEntries != fileInfo->undefed.numEntries ){
            continue;
        }
        if( _temp->numValues != fileInfo->numValues ){
            continue;
        }
        // Compare the "defined" ids
        if( fileInfo->defined.numEntries != 0 &&
            memcmp( _temp->defined.strings, fileInfo->defined.strings,
                    fileInfo->defined.numEntries ) != 0 ){
            continue;
        }
        // Compare the "undefined" ids
        if( fileInfo->undefed.numEntries != 0 &&
            memcmp( _temp->undefed.strings, fileInfo->undefed.strings,
                    fileInfo->undefed.numEntries ) != 0 ){
            continue;
        }
        if( fileInfo->numValues == 0 ){
            result = FALSE;
            break;
        }
        // Compare the values
        value = _temp->values.First();
        result = FALSE;
        while( value != NULL ){
            other = fileInfo->values.Find( value->macroNameID );
            if( other == NULL ){
                result = TRUE;
                break;
            }
            if( value->numParams != other->numParams ||
                value->length != other->length ||
                ( value->length != 0 &&
                memcmp( value->defn, other->defn, value->length ) != 0 ) ){
                result = TRUE;
                break;
            }
            value = _temp->values.Next();
        }
    }
    if( result ){
        if( headInfo == NULL ){
            _fileInfo->Insert( _temp->filenameID, _temp );
        } else {
            _temp->next = headInfo->next;
            headInfo->next = _temp;
        }
        _count += 1;
    } else {
        delete _temp;
        _state = IGNORE;
        _ignoreDepth = 1;
    }
    _temp = NULL;
    return result;
}


void DependList::EndFile()
/************************/
{
    if( _state == IGNORE ){
        _ignoreDepth -= 1;
        if( _ignoreDepth == 0 ){
            _state = USE;
        }
    }
}


void DependList::StartTemplate()
/******************************/
{
    if( _state != IGNORE ){
        _templateDepth += 1;
    }
}


void DependList::EndTemplate()
/****************************/
{
    if( _state != IGNORE && _templateDepth > 0 ){
        _templateDepth -= 1;
    }
}


WBool DependList::SaveTo( CacheOutFile *cache )
/*******************************************/
{
    FileGuardInfo       *currFile;
    DependValRec        *currValue;
    int                 i;
    int                 limit;

    cache->StartComponent( "Guards" );
    cache->AddDword( _count );
    currFile = _fileInfo->First();
    while( currFile != NULL ){
        cache->AddDword( currFile->filenameID );
        limit = currFile->defined.numEntries;
        cache->AddDword( limit );
        for( i=0; i<limit; i++ ){
            cache->AddDword( currFile->defined.strings[i] );
        }
        limit = currFile->undefed.numEntries;
        cache->AddDword( limit );
        for( i=0; i<limit; i++ ){
            cache->AddDword( currFile->undefed.strings[i] );
        }
        limit = currFile->numValues;
        cache->AddDword( limit );
        currValue = currFile->values.First();
        while( currValue != NULL ){
            cache->AddDword( currValue->macroNameID );
            cache->AddDword( currValue->numParams );
            cache->AddDword( currValue->length );
            cache->AddData( currValue->defn, currValue->length );
            currValue = currFile->values.Next();
        }
        if( currFile->next != NULL ){
            currFile = currFile->next;
        } else {
            currFile = _fileInfo->Next();
        }
    }
    cache->EndComponent();
    return TRUE;
}


WBool DependList::LoadFrom( CacheInFile *cache )
/********************************************/
{
    WBool               result;
    FileGuardInfo       *newFile;
    DependValRec        *newValue;
    int                 fileCount;
    FileGuardInfo       *lastFile;
    int                 limit,i,max;
    uint_32             id;


    result = cache->OpenComponent( "Guards" );
    if( !result ){
        return result;
    }

    cache->ReadDword( &fileCount );
    _count = fileCount;
    lastFile = NULL;
    while( fileCount > 0 ){
        newFile = new FileGuardInfo;
        cache->ReadDword( &id );
        newFile->filenameID = id;
        cache->ReadDword( &limit );
        max = limit>0?limit:BRM_NUM_IDS;
        newFile->defined.strings = new BRI_StringID[ max ];
        newFile->defined.maxEntries = max;
        newFile->defined.numEntries = limit;
        for( i=0; i<limit; i++ ){
            cache->ReadDword( &newFile->defined.strings[i] );
        }
        cache->ReadDword( &limit );
        max = limit>0?limit:BRM_NUM_IDS;
        newFile->undefed.strings = new BRI_StringID[ max ];
        newFile->undefed.maxEntries = max;
        newFile->undefed.numEntries = limit;
        for( i=0; i<limit; i++ ){
            cache->ReadDword( &newFile->undefed.strings[i] );
        }
        cache->ReadDword( &limit );
        newFile->numValues = limit;
        for( i=0; i<limit; i++ ){
            newValue = new DependValRec;
            cache->ReadDword( &newValue->macroNameID );
            cache->ReadDword( &newValue->numParams );
            cache->ReadDword( &newValue->length );
            if( newValue->length > 0 ){
                newValue->defn = new uint_8[newValue->length];
                cache->ReadData( newValue->defn, newValue->length );
            } else {
                newValue->defn = NULL;
            }
            newFile->values.Insert( newValue->macroNameID, newValue );
        }
        if( lastFile != NULL && lastFile->filenameID == id ){
            lastFile->next = newFile;
        } else {
            _fileInfo->Insert( id, newFile );
        }
        lastFile = newFile;
        fileCount--;
    }
    cache->CloseComponent();
    return result;
}


MacroList::MacroList()
/********************/
{
    _macroNames = new StringIDs(LIST_BLOCK);
}


MacroList::~MacroList()
/*********************/
{
    delete _macroNames;
}


WBool MacroList::HaveSeen( BRI_StringID macroName )
/***********************************************/
{
    int         i;

    for( i=0; i<_macroNames->numEntries; i++ ){
        if( _macroNames->strings[i] == macroName ){
            return TRUE;
        }
    }

    return FALSE;
}


void MacroList::AddMacroDecl( BRI_StringID macroName, BRI_SymbolID symID )
/************************************************************************/
{
    DeclRec     *decl;

    if( _macroNames->numEntries == _macroNames->maxEntries ){
        _macroNames->DoubleSize();
    }
    _macroNames->strings[ _macroNames->numEntries++ ] = macroName;

    decl = new DeclRec;
    decl->index = (BRI_SymbolID) symID;
    decl->attribs = BRI_SA_Macro;
    decl->nameID = macroName;
    decl->typeID = (BRI_TypeID) 0;

    CurrBrowser->declList()->Insert( decl );
}


WBool MacroList::SaveTo( CacheOutFile *cache )
/******************************************/
{
    int         i;

    cache->StartComponent( "Macros" );
    cache->AddDword( _macroNames->numEntries );
    for( i=0; i<_macroNames->numEntries; i++ ){
        cache->AddDword( _macroNames->strings[i] );
    }
    cache->EndComponent();
    return TRUE;
}


WBool MacroList::LoadFrom( CacheInFile *cache )
/*******************************************/
{
    WBool       result;
    int         limit,i;

    result = cache->OpenComponent( "Macros" );
    if( !result ){
        return result;
    }

    if( _macroNames->strings ){
        delete[] _macroNames->strings;
    }
    cache->ReadDword( &limit );
    _macroNames->maxEntries = limit;
    _macroNames->numEntries = limit;
    _macroNames->strings = new BRI_StringID[ limit ];
    for( i=0; i<limit; i++ ){
        cache->ReadDword( &_macroNames->strings[i] );
    }
    cache->CloseComponent();
    return result;
}
