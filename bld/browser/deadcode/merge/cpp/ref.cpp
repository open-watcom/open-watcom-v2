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
#include <dwarf.h>

// Project includes -------------------------------------------------------

#include "ref.h"
#include "errors.h"
#include "syminfo.h"

MergeReference::MergeReference( int mbrIndex,
                      uint_32 position,
                      uint_32 value,
                      uint_16 form,
                      SymbolInfo * symbol )
/*****************************************/
    : _position( position )
    , _value( value )
    , _form( form )
    , _mbrIndex( mbrIndex )
    , _symbol( symbol )
    , _newOffset( 0 )
{
}

MergeReference::~MergeReference()
/*********************/
{
}

int MergeReference::mbrIndex()
/***********************/
{
    return( _mbrIndex );
}

uint_32 MergeReference::position()
/***************************/
{
    return( _position );
}

uint_32 MergeReference::value()
/************************/
{
    return( _value );
}

uint_16 MergeReference::form()
/***********************/
{
    return( _form );
}

void MergeReference::setNewOffset( uint_32 newOffset )
/***********************************************/
{
    uint_8 * data;

    _newOffset = newOffset;
    _symbol->referenceResolved();

    data = _symbol->data();

    if( _form != DW_FORM_ref_addr ) {
        throw ErrInvalidFormat;
    }

    * ( uint_32 * )( data + _position ) = newOffset;
}

uint_32 MergeReference::newOffset()
/****************************/
{
    return( _newOffset );
}

#if DEBUG_DUMP
void MergeReference::dumpData()
/************************/
{
    printf( "<%d,%lx,%lx>\n", _mbrIndex, _position, _value );
}
#endif
