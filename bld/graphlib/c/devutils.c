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


#include "gdefn.h"


short _GraphMode( void )
//======================

// make sure we are in a graphics mode

{
    _ErrorStatus = _GROK;
    if( !_GrMode ) {
        _ErrorStatus = _GRNOTINPROPERMODE;
    }
    return( _GrMode );
}


short _GrProlog( void )
//=====================

// same as GraphMode, except it also turns off the cursor

{
    _ErrorStatus = _GROK;
    if( !_GrMode ) {
        _ErrorStatus = _GRNOTINPROPERMODE;
    } else {
        _CursorOff();
    }
    return( _GrMode );
}


void _GrEpilog( void )
//====================

// conditionally turn the cursor back on

{
    if( _CursState == _GCURSORON ) {
        _CursorOn();
    }
}


short _InitDevice( short mode )
//=============================

// call the 'init' routine for the given graphics device

{
    gr_device _FARD     *dev_ptr;

    dev_ptr = _CurrState->deviceptr;
    return( ( *dev_ptr->init )( mode ) );
}


void _FiniDevice( void )
//======================

// call the 'fini' routine for the current graphics device

{
    gr_device _FARD     *dev_ptr;

    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->finish )();
}


void _StartDevice( void )
//=======================

// call the 'set' routine for the current graphics device

{
    gr_device _FARD     *dev_ptr;

    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->set )() ;
}


void _ResetDevice( void )
//=======================

// call the 'reset' routine for the current graphics device

{
    gr_device _FARD     *dev_ptr;

    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->reset )() ;
}
