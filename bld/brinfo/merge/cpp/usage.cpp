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
#include "handle.hpp"
#include "reindex.hpp"
#include "scopes.hpp"
#include "usage.hpp"
#include "cache.hpp"

// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions
#pragma warning 549 9
static Pool UsageRec::_usagePool( UsagePool, sizeof(UsageRec) );
#pragma warning 549 3


UsageList::UsageList()
/********************/
{
    _count = 0;
    _firstFile = NULL;
    _curFile = NULL;
}


UsageList::~UsageList()
/*********************/
{
    FileData *  currFile;
    FileData *  prevFile;
    UsageRec *  currLine;
    UsageRec *  currCol;
    UsageRec *  prevCol;

    prevFile = NULL;
    currFile = _firstFile;
    while( currFile != NULL ){
        currLine = currFile->lines.First();
        while( currLine != NULL ) {
            prevCol = NULL;
            currCol = currLine->next;
            while( currCol != NULL ){
                prevCol = currCol;
                currCol = currCol->next;
                delete prevCol;
            }
            delete currLine;
            currLine = currFile->lines.Next();
        }
        prevFile = currFile;
        currFile = currFile->next;
        delete prevFile;
    }
}


void UsageList::Insert( UsageRec * data )
/***************************************/
{
    UsageRec *  current;
    UsageRec *  prev;

    if( _firstFile == NULL ){
        _firstFile = new FileData;
        _firstFile->next = NULL;
        _firstFile->filename = (BRI_StringID) 0;
        _firstFile->curLine = 0;
        _firstFile->curColumn = 0;
        _curFile = _firstFile;
    }

    data->column = _curFile->curColumn;

    // Find the right line and column...
    current = _curFile->lines.Find( _curFile->curLine );
    prev = NULL;
    while( current != NULL && current->column < _curFile->curColumn ){
        prev = current;
        current = current->next;
    }
    data->next = current;
    if( prev == NULL ){
        _curFile->lines.Insert( _curFile->curLine, data, TRUE );
    } else {
        prev->next = data;
    }
    _count += 1;
}


void UsageList::Delta( int_8 dcol, int_16 dline )
/***********************************************/
{
    _curFile->curLine += dline;
    _curFile->curColumn += dcol;
}


void UsageList::SetFile( BRI_StringID filename )
/**********************************************/
{
    FileData *  curr;
    FileData *  prev;
    FileData *  newfile;

    prev = NULL;
    curr = _firstFile;
    while( curr != NULL && curr->filename < filename ){
        prev = curr;
        curr = curr->next;
    }
    if( _curFile != NULL ){
        _openFileStack.Push( _curFile );
    }
    if( curr == NULL || curr->filename > filename ){
        newfile = new FileData;
        newfile->next = curr;
        newfile->filename = filename;
        newfile->curLine = 0;
        newfile->curColumn = 0;
        if( prev == NULL ){
            _firstFile = newfile;
        } else {
            prev->next = newfile;
        }
        _curFile = newfile;
    } else {
        curr->curLine = 0;
        curr->curColumn = 0;
        _curFile = curr;
    }
}


void UsageList::EndFile()
/***********************/
{
    _curFile = _openFileStack.Pop();
}


UsageRec * UsageList::First()
/***************************/
{
    UsageRec *  result = NULL;

    _curFile = _firstFile;
    while( _curFile != NULL && _curFile->lines.Empty() ){
        _curFile = _curFile->next;
    }
    if( _curFile != NULL ){
        result = _curFile->lines.First();
        _curUsage = result;
    }
    return result;
}


UsageRec * UsageList::Next()
/**************************/
{
    UsageRec *  result = NULL;

    if( _curFile != NULL ){
        if( _curUsage != NULL ){
            _curUsage = _curUsage->next;
            if( _curUsage != NULL ){
                _curFile->curColumn = _curUsage->column;
                result = _curUsage;
            } else {
                _curUsage = _curFile->lines.Next();
                if( _curUsage != NULL ){
                    _curFile->curLine = _curFile->lines.CurrentKey();
                    _curFile->curColumn = _curUsage->column;
                    result = _curUsage;
                } else {
                    do {
                        _curFile = _curFile->next;
                    } while( _curFile != NULL && _curFile->lines.Empty() );

                    if( _curFile != NULL ){
                        _curUsage = _curFile->lines.First();
                        _curFile->curLine = _curFile->lines.CurrentKey();
                        _curFile->curColumn = _curUsage->column;
                        result = _curUsage;
                    }
                }
            }
        }
    }
    return result;
}


BRI_StringID UsageList::GetFileName()
/***********************************/
{
    if( _curFile == NULL ){
        return (BRI_StringID) 0;
    } else {
        return _curFile->filename;
    }
}


uint_32 UsageList::GetLine()
/**************************/
{
    if( _curFile == NULL ){
        return 0;
    } else {
        return _curFile->curLine;
    }
}


uint_32 UsageList::GetColumn()
/****************************/
{
    if( _curFile == NULL ){
        return 0;
    } else {
        return _curFile->curColumn;
    }
}


UsageRec * UsageList::At( BRI_StringID fname, uint_32 line,
                          uint_32 column )
/*********************************************************/
{
    UsageRec *  result;
    FileData *  curFile;
    UsageRec *  curLine;
    UsageRec *  curColumn;
    UsageRec *  prevColumn;
    WBool       found = FALSE;

    // Find the right file...
    curFile = _firstFile;
    while( curFile != NULL && curFile->filename != fname ){
        curFile = curFile->next;
    }

    if( curFile != NULL ){
        curLine = curFile->lines.Find( line );
        curColumn = curLine;
        prevColumn = NULL;
        while( curColumn != NULL && curColumn->column < column ){
            prevColumn = curColumn;
            curColumn = curColumn->next;
        }

        if( curColumn == NULL ){
            _right = curFile->lines.FindRight( line );
        } else {
            _right = curColumn;
            if( curColumn->column == column ){
                found = TRUE;
            }
        }
        if( !found ){
            if( prevColumn != NULL ){
                _left = curLine;
                while( _left->column < prevColumn->column ){
                    _left = _left->next;
                }
            } else {
                prevColumn = curFile->lines.FindLeft( line );
                if( prevColumn != NULL ){
                    _left = prevColumn;
                    while( prevColumn->next != NULL ){
                        prevColumn = prevColumn->next;
                        if( prevColumn->column != _left->column ){
                            _left = prevColumn;
                        }
                    }
                } else {
                    _left = NULL;
                }
            }
        }
    } else {
        _left = _right = NULL;
    }


    // If we found the exact line/column requested, return the
    // first reference at that position.  Otherwise, return NULL
    // and prepare to iterate over the references at the next
    // valid line/column positions to the right and left.

    if( found ){
        result = _right;
        _right = _right->next;
    } else {
        result = NULL;
    }
    return result;
}


UsageRec * UsageList::NextAt()
/****************************/
{
    UsageRec *  result = NULL;
    uint_32     column;

    // Return the next reference at the same line/column.
    // Assumes that a previous call to UsageList::At returned
    // a non-NULL value.

    result = _right;
    if( result != NULL ){
        column = result->column;
        _right = result->next;
        if( _right != NULL && _right->column != column ){
            _right = NULL;
        }
    }
    return result;
}


UsageRec * UsageList::NextAtLeft()
/********************************/
{
    UsageRec *  result = NULL;
    uint_32     column;

    // Return the next reference at the line/column position
    // to the left.  Assumes that a previous call to UsageList::At
    // returned NULL.

    result = _left;
    if( result != NULL ){
        column = result->column;
        _left = result->next;
        if( _left != NULL && _left->column != column ){
            _left = NULL;
        }
    }
    return result;
}


UsageRec * UsageList::NextAtRight()
/*********************************/
{
    UsageRec *  result = NULL;
    uint_32     column;

    // Return the next reference at the line/column position
    // to the right.  Assumes that a previous call to UsageList::At
    // returned NULL.

    result = _right;
    if( result != NULL ){
        column = result->column;
        _right = result->next;
        if( _right != NULL && _right->column != column ){
            _right = NULL;
        }
    }
    return result;
}


void UsageList::Reset()
/*********************/
{
    FileData    *current;

    _curFile = NULL;
    _curUsage = NULL;
    _left = NULL;
    _right = NULL;

    current = _firstFile;
    while( current != NULL ){
        current->curLine = 0;
        current->curColumn = 0;
        current = current->next;
    }
}


// Flags used by SaveTo and LoadFrom
enum  SaveLoadFlags {
    END_FLAG    = 0x00,
    FILE_FLAG   = 0x01,
    LINE_FLAG   = 0x02,
    COLUMN_FLAG = 0x03,
};

static const int SIZE_SL_FLAGS = 1;


WBool UsageList::SaveTo( CacheOutFile *cache )
/******************************************/
{
    FileData    *current;
    UsageRec    *currLine;
    UsageRec    *currColumn;

    cache->StartComponent( "Usages" );
    cache->AddDword( _count );
    current = _firstFile;
    while( current != NULL ){
        cache->AddByte( FILE_FLAG );
        cache->AddDword( current->filename );
        cache->AddDword( current->curLine );
        cache->AddDword( current->curColumn );
        currLine = current->lines.First();
        while( currLine != NULL ){
            cache->AddByte( LINE_FLAG );
            cache->AddDword( current->lines.CurrentKey() );
            cache->AddDword( currLine->column );
            cache->AddData( &currLine->type, BRI_SIZE_REFERENCETYPE );
            cache->AddDword( currLine->targetID );
            cache->AddDword( currLine->enclosing );
            currColumn = currLine->next;
            while( currColumn != NULL ){
                cache->AddByte( COLUMN_FLAG );
                cache->AddDword( currColumn->column );
                cache->AddData( &currColumn->type, BRI_SIZE_REFERENCETYPE );
                cache->AddDword( currColumn->targetID );
                cache->AddDword( currColumn->enclosing );
                currColumn = currColumn->next;
            }
            currLine = current->lines.Next();
        }
        current = current->next;
    }
    cache->EndComponent();
    return TRUE;
}


WBool UsageList::LoadFrom( CacheInFile *cache )
/*******************************************/
{
    WBool               result;
    uint_8              flag;
    uint_32             count;
    uint_32             lineNum;
    FileData            *current;
    UsageRec            *lastColumn;
    FileData            *newFile;
    UsageRec            *newLine;
    UsageRec            *newColumn;

    result = cache->OpenComponent( "Usages" );

    if( !result ){
        return result;
    }

    if( _openFileStack.Count() > 0 ){
        _openFileStack.Clear();
    }

    cache->ReadDword( &count );
    _count = count;
    current = NULL;
    lastColumn = NULL;
    while( count > 0 ){
        cache->ReadByte( flag );
        switch( flag ){
            case FILE_FLAG:
                if( lastColumn != NULL ){
                    lastColumn->next = NULL;
                }
                newFile = new FileData;
                cache->ReadDword( &newFile->filename );
                cache->ReadDword( &newFile->curLine );
                cache->ReadDword( &newFile->curColumn );
                newFile->next = NULL;
                if( current != NULL ){
                    current->next = newFile;
                } else {
                    _firstFile = newFile;
                }
                current = newFile;
            break;

            case LINE_FLAG:
                if( lastColumn != NULL ){
                    lastColumn->next = NULL;
                }
                newLine = new UsageRec;
                cache->ReadDword( &lineNum );
                cache->ReadDword( &newLine->column );
                newLine->type = (BRI_ReferenceType) 0;
                cache->ReadData( &newLine->type, BRI_SIZE_REFERENCETYPE );
                cache->ReadDword( &newLine->targetID );
                cache->ReadDword( &newLine->enclosing );
                current->lines.Insert( lineNum, newLine );
                lastColumn = newLine;
                count--;
            break;

            case COLUMN_FLAG:
                newColumn = new UsageRec;
                cache->ReadDword( &newColumn->column );
                newColumn->type = (BRI_ReferenceType) 0;
                cache->ReadData( &newColumn->type, BRI_SIZE_REFERENCETYPE );
                cache->ReadDword( &newColumn->targetID );
                cache->ReadDword( &newColumn->enclosing );
                lastColumn->next = newColumn;
                lastColumn = newColumn;
                count--;
            break;
        }
    }
    if( lastColumn != NULL ){
        lastColumn->next = NULL;
    }
    cache->CloseComponent();

    return result;
}
