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


#include "module.h"
#include "varsym.h"

const int POOLSIZE = 16;

#pragma warning 549 5           // sizeof contains compiler genned info.
MemoryPool VariableSym::_pool( sizeof( VariableSym ), "VariableSym", POOLSIZE );
#pragma warning 549 3

void * VariableSym::operator new( size_t )
//----------------------------------------
{
    return( _pool.alloc() );
}

void VariableSym::operator delete( void * mem )
//---------------------------------------------
{
    _pool.free( mem );
}

#if 0 // remove this, put in fullyScoped parm to description
void VariableSym::loadTypeInfo( WVList & parts )
//----------------------------------------------
{
    Module * module = getModule();

    _description = &parts;
    module->setModule();
    DRDecoratedNameList( this, getHandle(), NULL, descCallBack );
}
#endif
