/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CDC functions requiring Windows 2000 or
*               later.
*
****************************************************************************/


#include "stdafx.h"

// This file contains only those functions that require Windows 2000 or later.
// Do not add any functions that run on earlier versions or any functions that
// require a later version to this file.  Doing so will break compatibility.

DWORD CDC::SetLayout( DWORD dwLayout )
/************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetLayout( m_hDC, dwLayout );
        return( ::SetLayout( m_hAttribDC, dwLayout ) );
    }
    return( ::SetLayout( m_hDC, dwLayout ) );
}

