/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Describes a group of panel controls
*
****************************************************************************/


#include "wipfc.hpp"
#include "ctrlgrp.hpp"
#include "errors.hpp"
#include "outfile.hpp"


dword ControlGroup::write( OutFile* out ) const
{
    std::size_t size = static_cast< word >( _buttonIndex.size() );
    // items count
    if( out->put( static_cast< word >( size ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _buttonIndex ) )
        throw FatalError( ERR_WRITE );
    return( static_cast< dword >( sizeof( word ) + size * sizeof( word ) ) );
}
