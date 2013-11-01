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
* Description:  Implementation of CCtrlView.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CCtrlView, CView )

BEGIN_MESSAGE_MAP( CCtrlView, CView )
    ON_WM_PAINT()
END_MESSAGE_MAP()

CCtrlView::CCtrlView( LPCTSTR lpszClass, DWORD dwStyle )
/******************************************************/
{
    m_strClass = lpszClass;
    m_dwDefaultStyle = dwStyle;
}

void CCtrlView::OnDraw( CDC *pDC )
/********************************/
{
    // Control views don't need to be drawn.  This override exists solely because
    // OnDraw is pure virtual in CView.  It should never actually be called.
    UNUSED_ALWAYS( pDC );
}

BOOL CCtrlView::PreCreateWindow( CREATESTRUCT &cs )
/*************************************************/
{
    cs.lpszClass = m_strClass;
    cs.style = m_dwDefaultStyle;
    return( CView::PreCreateWindow( cs ) );
}

#ifdef _DEBUG

void CCtrlView::Dump( CDumpContext &dc ) const
/********************************************/
{
    CView::Dump( dc );

    dc << "m_strClass = " << m_strClass << "\n";
    dc << "m_dwDefaultStyle = " << m_dwDefaultStyle << "\n";
}

#endif // _DEBUG

void CCtrlView::OnPaint()
/***********************/
{
    // Controls paint themselves, so WM_PAINT should be sent directly to the control
    // instead of being handled by CView::OnPaint.
    Default();
}
