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


// System includes ------------------------------------------------------
#include <stdarg.h>

// Project includes -----------------------------------------------------

#include "strtable.h"

// Methods --------------------------------------------------------------

StringTable::StringTable( int numCols, ... )
    : _columns( numCols )
    , _colWidths( NULL )
    , _totalWidth( 0 )
    , _buffer( NULL )
    , _cbStringTable( NULL )
/******************************************/
{
    int i;
    va_list colParms;

    _colWidths = new int[ numCols ];

    va_start( colParms, numCols );

    for( i = 0; i < numCols; i += 1 ) {
        _colWidths[ i ] = va_arg( colParms, int );
        _totalWidth += _colWidths[ i ];
    }

    _buffer = new char[ _totalWidth + 1 ];

    // position is an array of ptrs to keep track of where we are in each
    // string; it is only used in printRow
    _position = new (const char *[ _columns ]);

    va_end( colParms );
}

StringTable::~StringTable()
/*************************/
{
    int i;

    for( i = 0; i < _table.count(); i += 1 ) {
        delete [] _table[ i ];
    }

    _table.reset();

    delete [] _colWidths;
    delete [] _buffer;
    delete [] _position;
}

void StringTable::setOutputCallback( WObject * client, CBStringTable cb )
/***********************************************************************/
{
    _client = client;
    _cbStringTable = cb;

}

void StringTable::add( const char * firstString, ... )
/****************************************************/
{
    va_list args;
    int i;
    char const ** row;

    row = new (const char *[ _columns ]);

    row[ 0 ] = firstString;

    va_start( args, firstString );

    for( i = 1; i < _columns; i += 1 ) {
        row[ i ] = va_arg( args, char * );
    }

    va_end( args );

    _table.add( row );
}

void StringTable::replaceAt( int rowNum, const char * firstString, ... )
/**********************************************************************/
{
    va_list args;
    int i;
    char const ** row;

    if( rowNum >= 0 && rowNum < _table.count() ) {

        row = _table[ rowNum ];

        row[ 0 ] = firstString;

        va_start( args, firstString );

        for( i = 1; i < _columns; i += 1 ) {
            row[ i ] = va_arg( args, char * );
        }

        va_end( args );
    }
}

void StringTable::outputRow( const char * firstString, ... )
/**********************************************************/
// Number of strings expected is the number of columns
{
    va_list args;
    int i;
    char const ** row;

    row = new (const char *[ _columns ]);

    row[ 0 ] = firstString;

    va_start( args, firstString );

    for( i = 1; i < _columns; i += 1 ) {
        row[ i ] = va_arg( args, char * );
    }

    va_end( args );
    printRow( row );

    delete [] row;
}

void StringTable::output()
/************************/
{
    int i;

    for( i = 0; i < _table.count(); i += 1 ) {
        printRow( _table[ i ]);
    }
}

void StringTable::printRow( const char * row[] )
/**********************************************/
// row is an array of strings; each string corresponds to one
// column
{
    int i, j, k;
    int numDone;
    char * bufPos;

    for( i = 0; i < _columns; i += 1 ) {
        _position[ i ] = row[ i ];
    }

    // loop until the contents of each column is outputted
    for( numDone = 0; numDone < _columns; ) {
        bufPos = _buffer;

        for( i = 0; i < _columns; i += 1 ) {
            if( _position[ i ] != NULL ) {
                for( j = 0; j < _colWidths[ i ] && *(_position[ i ]) != '\0'; j++ ) {
                    *bufPos = *(_position[ i ]);
                    (_position[ i ]) += 1;
                    bufPos += 1;
                }

                if( *(_position[ i ]) == '\0' ) {
                    numDone += 1;
                }

                for( k = j; k < _colWidths[ i ]; k += 1 ) {
                    *bufPos = ' ';
                    bufPos += 1;
                }
            } else {
                numDone += 1;
                for( j = 0; j < _colWidths[ i ]; j += 1 ) {
                    *bufPos = ' ';
                    bufPos += 1;
                }
            }

        }

        *bufPos = '\0';

        if( _cbStringTable && _client ) {
            WString tmp( _buffer );
            (_client->*_cbStringTable) ( tmp );
        }
    }
}
