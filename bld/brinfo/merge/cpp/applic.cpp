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


/*
    applic.cpp

*/

#include "wpch.hpp"
#include "applic.hpp"


// Code added here will be included at the top of the .CPP file

//  Include definitions for resources.
#include "WRes.h"

ApplicationClass::ApplicationClass()
{
    SetEventHandler( WProcessAttachEvent, this,
        (WEventHandler) ProcessAttachHandler );

    SetEventHandler( WProcessDetachEvent, this,
        (WEventHandler) ProcessDetachHandler );

    SetEventHandler( WThreadAttachEvent, this,
        (WEventHandler) ThreadAttachHandler );

    SetEventHandler( WThreadDetachEvent, this,
        (WEventHandler) ThreadDetachHandler );
}
ApplicationClass::~ApplicationClass()
{
}
WBool ApplicationClass::ProcessAttachHandler( WObject *, WProcessAttachEventData * )
{
    return FALSE;
}
WBool ApplicationClass::ProcessDetachHandler( WObject *, WProcessDetachEventData * )
{
    return FALSE;
}
WBool ApplicationClass::ThreadAttachHandler( WObject *, WThreadAttachEventData * )
{
    return FALSE;
}
WBool ApplicationClass::ThreadDetachHandler( WObject *, WThreadDetachEventData * )
{
    return FALSE;
}
