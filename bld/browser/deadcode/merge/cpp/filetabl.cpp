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


// System includes --------------------------------------------------------

#include <stdio.h>

// Project includes -------------------------------------------------------

#include "filetabl.h"
#include "errors.h"

/*
 | Note that file indices start at 1 because of the DWARF standard
 | (not the mbr file index, of course, since that has nothing to do
 | with DWARF data -- it's the mbr file index associated with the entry
 | being processed)
 */
FileTable::FileTable()
/********************/
    : _index( 1 )
{
}

FileTable::~FileTable()
/*********************/
{
    int i;
    int j;

    #if DEBUG_LIST
        printf( "FileTable::~FileTable() -- _repTable contains %d lists\n", _repTable.entries() );
        for( i = 0; i < _repTable.entries(); i += 1 ) {
            printf( "                        -- _repTable[%03d] contains %d elements\n", i, _repTable[i]->entries() );
        }
        printf( "                        -- _globalTable contains %d elements\n", _globalTable.entries() );
    #endif

    for( i = 0; i < _repTable.entries(); i += 1 ) {
        for( j = 0; j < _repTable[ i ]->entries(); j += 1 ) {
            delete (*_repTable[ i ])[ j ];
        }
        delete _repTable[ i ];
    }
}

bool FileTable::add( int mbrIndex, char * name, void * data )
/***********************************************************/
// Returns TRUE if entry added to global table,
//        FALSE is entry already exists (so only added to replacement table)
{
    int i;
    void * other;
    int otherIndex;
    bool rc;

    for( i = _repTable.entries(); i <= mbrIndex; i += 1 ) {
        IntegerVector * ilist;

        ilist = new IntegerVector;
        if( ilist == NULL ) {
            throw ErrOutOfMemory;
        }
        _repTable.insert( ilist );
    }

    _strTable.find( name, &other, otherIndex );

    int * pi;

    if( other == NULL ) {
        if( otherIndex == mbrIndex ) {
            printf( "Error!  Same index!\n" );
        }

        _strTable.add( name, data, _index );
        _globalTable.insert( data );
        pi = new int( _index );
        _repTable[ mbrIndex ]->insert( pi );
        _index++;
        rc = TRUE;
    } else {
        pi = new int( otherIndex );
        _repTable[ mbrIndex ]->insert( pi );
        rc = FALSE;
    }
    return( rc );
}

void * FileTable::find( char * name )
/***********************************/
{
    void * other;
    int otherIndex;

    _strTable.find( name, &other, otherIndex );
    return other;
}

void * FileTable::find( int globalIndex )
/***************************************/
{
    return _globalTable[ globalIndex ];
}

void * FileTable::operator[]( int globalIndex )
/*********************************************/
{
    return _globalTable[ globalIndex ];
}

int FileTable::find( int mbrIndex, int subIndex )
/***********************************************/
{
    #if DEBUG
    if( mbrIndex < 0 || mbrIndex >= _repTable.entries() ) {
        printf( "FileTable::find( %d, %d ) -- mbrIndex out of range.\n",
                mbrIndex, subIndex );
    }

    IntegerVector * list = _repTable[ mbrIndex ];

    if( subIndex - 1 < 0 || subIndex - 1 >= list->entries() ) {
        printf( "FileTable::find( %d, %d ) -- subIndex out of range.\n",
                mbrIndex, subIndex );
    }
    #endif

    /*
     | Subtract 1 because we index from 0, DWARF from 1
     */
    return *((*(_repTable[ mbrIndex ]))[ subIndex - 1 ]);
}

int FileTable::count()
/********************/
{
    return _globalTable.entries();
}

#if DEBUG_DUMP
void FileTable::dumpReplacementTable()
/************************************/
{
    printf( "+---- Replacement Table -----------------------------+\n" );

    int i, j;

    for( i = 0; i < _repTable.entries(); i += 1 ) {
        for( j = 0; j < _repTable[ i ]->entries(); j += 1 ) {
            printf( "  <%d,%d> = %d\n", i, j, *((*(_repTable[ i ]))[ j ]));
        }
    }

    printf( "+----------------------------------------------------+\n" );
}
#endif
