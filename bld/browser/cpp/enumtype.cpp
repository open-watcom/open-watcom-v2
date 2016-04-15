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


#include <drenum.h>
#include "enumtype.h"
#include "mem.h"
#include "optmgr.h"
#include "util.h"

const int POOLSIZE = 32;

#pragma warning 549 9           // sizeof contains compiler genned info.
MemoryPool EnumType::_pool( sizeof( EnumType ), "EnumType", POOLSIZE );
#pragma warning 549 3

struct searchData {
    EnumType *  me;
    WVList *    list;
};

EnumType::EnumType( dr_handle drhdl, dr_handle drhdl_prt, Module * m, char * name )
            : Symbol( drhdl, drhdl_prt, m, name )
//---------------------------------------------------------------------
{
    _byteSize = DRGetByteSize( getHandle() );
}

void * EnumType::operator new( size_t )
//-------------------------------------
{
    return( _pool.alloc() );
}

void EnumType::operator delete( void * mem )
//------------------------------------------
{
    _pool.free( mem );
}

void EnumType::loadElements( WVList & list )
//------------------------------------------
{
    searchData data;
    data.me = this;
    data.list = &list;

    DRLoadEnum( getHandle(), &data, EnumType::loadHook );
}

static bool EnumType::loadHook( char * name, unsigned_32 val, void * data )
//-------------------------------------------------------------------------
{
    EnumElement *   elem;
    searchData *    info = (searchData *) data;

    elem = new EnumElement( name, info->me->_byteSize, val );
    info->list->add( elem );

    return true;
}

//------------------------ EnumElement -------------------------------------

static char *FormArray[] = {
    "0x%lx",    // 0xab
    "0X%lX",    // 0XAB
    "0x%lX",    // 0xAB
    "0%lo",     // 0123
    "%ld",      // 123
    "'%lc'"     // 'b'
};


EnumElement::EnumElement( char * name, unsigned byteSize, unsigned_32 val )
        : _name( name )
        , _byteSize( byteSize )
        , _value( val )
//-------------------------------------------------------------------------
{
}

EnumElement::~EnumElement()
//-------------------------
{
    delete [] _name;
}

WString & EnumElement::value( WString & str )
//-------------------------------------------
{
    EnumViewStyle style = WBRWinBase::optManager()->getEnumStyle();

    if( style == EV_Character && !isprint( (char) _value ) ){
        style = EV_HexMixedCase;
    }

    str.printf( FormArray[ style ], _value );
    return str;
}

char * EnumElement::name( char * )
//--------------------------------
{
    return _name;
}
