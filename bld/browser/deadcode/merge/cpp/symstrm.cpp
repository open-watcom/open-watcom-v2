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

#include "symstrm.h"

SymbolStream::SymbolStream( char * filename )
/*******************************************/
    : _outFile( new MFile( filename ))
    , _offset( 0 )
{
}

SymbolStream::~SymbolStream()
/***************************/
{
    #if DEBUG_LIST
        printf( "SymbolStream::~SymbolStream() -- _symList contains %d entries\n", _symList.count() );
        printf( "                              -- _syms contains %d entries\n", _syms.entries() );
    #endif

    if( _outFile ) {
        delete _outFile;
    }
}

void SymbolStream::open()
/***********************/
{
    _outFile->open( MFileOWriteB );
}

void SymbolStream::close()
/************************/
{
    _outFile->close();
}

void SymbolStream::addSymbol( SymbolInfo * sym )
/**********************************************/
{
    _symList.addRight( sym );

    /*
     | Note that we're assuming that the length doesn't change.  This
     | could potentially cause problems if, for example, we need to
     | change from a ref4 to a ref8 somewhere in the symbol!
     */
    _offset += sym->length();

    flush();
}

unsigned long SymbolStream::offset()
/**********************************/
{
    return( _offset );
}

int SymbolStream::count()
/***********************/
{
    return( _symList.count() );
}

SymbolInfo * SymbolStream::operator[]( int index )
/************************************************/
{
    return( _symList[ index ]);
}

void SymbolStream::setOffset( unsigned long offset )
/**************************************************/
{
    _offset = offset;
}

void SymbolStream::flush()
/************************/
{
    int i;
    int flushed;

    flushed = 0;

    for( i = 0; i < _symList.count(); i += 1 ) {
        if( _symList[ i ]->numUnresolved() == 0 ) {
            _outFile->write( _symList[ i ]->data(), _symList[ i ]->length() );
            flushed += 1;
        } else {
            break;
        }
    }

    for( i = 0; i < flushed; i += 1 ) {
        delete _symList.deleteLeft();
    }
}
