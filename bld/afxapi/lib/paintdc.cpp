/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CPaintDC.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CPaintDC, CDC )

CPaintDC::CPaintDC( CWnd *pWnd )
/******************************/
{
    m_hWnd = pWnd->GetSafeHwnd();
    m_hDC = ::BeginPaint( m_hWnd, &m_ps );
    if( m_hDC == NULL ) {
        throw new CResourceException;
    }
    m_hAttribDC = m_hDC;
}

CPaintDC::~CPaintDC()
/*********************/
{
    ::EndPaint( m_hWnd, &m_ps );
}

#ifdef _DEBUG

void CPaintDC::Dump( CDumpContext &dc ) const
/*******************************************/
{
    CDC::Dump( dc );

    dc << "m_hWnd = " << m_hWnd << "\n";
    dc << "m_ps.hdc = " << m_ps.hdc << "\n";
    dc << "m_ps.fErase = " << m_ps.fErase << "\n";
    dc << "m_ps.rcPaint = " << m_ps.rcPaint << "\n";
}

#endif // _DEBUG
