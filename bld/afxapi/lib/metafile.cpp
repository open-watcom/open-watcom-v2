/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CMetaFileDC.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CMetaFileDC, CDC )

CMetaFileDC::CMetaFileDC()
/************************/
{
    m_bEnhanced = FALSE;
}

CMetaFileDC::~CMetaFileDC()
/*************************/
{
    // If a CMetaFileDC is deleted before the metafile is closed, close and delete the
    // metafile in the destructor.  Use m_bEnhanced to keep track of whether it's a
    // standard or enhanced metafile.  (Microsoft's implementation doesn't do this and
    // instead always assumes a standard metafile.)
    if( m_hDC != NULL ) {
        if( m_bEnhanced ) {
            HENHMETAFILE hEMF = ::CloseEnhMetaFile( m_hDC );
            ::DeleteEnhMetaFile( hEMF );
        } else {
            HMETAFILE hMF = ::CloseMetaFile( m_hDC );
            ::DeleteMetaFile( hMF );
        }
        m_hDC = NULL;
    }
}

int CMetaFileDC::GetClipBox( LPRECT lpRect ) const
/************************************************/
{
    if( m_hAttribDC != NULL ) {
        return( ::GetClipBox( m_hAttribDC, lpRect ) );
    } else {
        return( ::GetClipBox( m_hDC, lpRect ) );
    }
}

BOOL CMetaFileDC::PtVisible( int x, int y ) const
/***********************************************/
{
    if( m_hAttribDC != NULL ) {
        return( ::PtVisible( m_hAttribDC, x, y ) );
    } else {
        return( ::PtVisible( m_hDC, x, y ) );
    }
}

BOOL CMetaFileDC::RectVisible( LPCRECT lpRect ) const
/***************************************************/
{
    if( m_hAttribDC != NULL ) {
        return( ::RectVisible( m_hAttribDC, lpRect ) );
    } else {
        return( ::RectVisible( m_hDC, lpRect ) );
    }
}

void CMetaFileDC::ReleaseOutputDC()
/*********************************/
{
    // This should never be called.
}

void CMetaFileDC::SetOutputDC( HDC hDC )
/**************************************/
{
    // This should never be called.
    UNUSED_ALWAYS( hDC );
}

#ifdef _DEBUG

void CMetaFileDC::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CDC::Dump( dc );

    dc << "m_bEnhanced = " << m_bEnhanced << "\n";
}

#endif // _DEBUG
