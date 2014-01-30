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
* Description:  Implementation of CImageList.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

IMPLEMENT_DYNAMIC( CImageList, CObject )

CImageList::CImageList()
/**********************/
{
    m_hImageList = NULL;
}

CImageList::~CImageList()
/***********************/
{
    DeleteImageList();
}

#ifdef _DEBUG

void CImageList::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CObject::Dump( dc );

    dc << "m_hImageList = " << m_hImageList << "\n";
}

#endif // _DEBUG

BOOL CImageList::Attach( HIMAGELIST hImageList )
/**********************************************/
{
    ASSERT( m_hImageList == NULL );
    if( hImageList == NULL ) {
        return( FALSE );
    }
    m_hImageList = hImageList;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHIMAGELIST;
    ASSERT( pHandleMap != NULL );
    pHandleMap->SetPermanent( hImageList, this );
    return( TRUE );
}

HIMAGELIST CImageList::Detach()
/*****************************/
{
    if( m_hImageList == NULL ) {
        return( NULL );
    }
    HIMAGELIST hImageList = m_hImageList;
    m_hImageList = NULL;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHIMAGELIST;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( hImageList );
    return( hImageList );
}

BOOL CImageList::DrawIndirect( CDC *pDC, int nImage, POINT pt, SIZE sz, POINT ptOrigin,
                               UINT fStyle, DWORD dwRop, COLORREF rgbBack,
                               COLORREF rgbFore, DWORD fState, DWORD Frame,
                               COLORREF crEffect )
/************************************************/
{
    IMAGELISTDRAWPARAMS imldp;
    imldp.cbSize = sizeof( IMAGELISTDRAWPARAMS );
    imldp.himl = m_hImageList;
    imldp.i = nImage;
    imldp.hdcDst = pDC->GetSafeHdc();
    imldp.x = pt.x;
    imldp.y = pt.y;
    imldp.cx = sz.cx;
    imldp.cy = sz.cy;
    imldp.xBitmap = ptOrigin.x;
    imldp.yBitmap = ptOrigin.y;
    imldp.rgbBk = rgbBack;
    imldp.rgbFg = rgbFore;
    imldp.fStyle = fStyle;
    imldp.dwRop = dwRop;
    imldp.fState = fState;
    imldp.Frame = Frame;
    imldp.crEffect = crEffect;
    return( ::ImageList_DrawIndirect( &imldp ) );
}

BOOL CImageList::Read( CArchive *pArchive )
/*****************************************/
{
    ASSERT( pArchive != NULL );
    CArchiveStream ars( pArchive );
    return( Attach( ::ImageList_Read( &ars ) ) );
}

BOOL CImageList::Write( CArchive *pArchive )
/******************************************/
{
    ASSERT( pArchive != NULL );
    CArchiveStream ars( pArchive );
    return( ::ImageList_Write( m_hImageList, &ars ) );
}

void PASCAL CImageList::DeleteTempMap()
/*************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHIMAGELIST;
    ASSERT( pHandleMap != NULL );
    pHandleMap->DeleteTemp();
}

CImageList * PASCAL CImageList::FromHandle( HIMAGELIST hImageList )
/*****************************************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHIMAGELIST;
    ASSERT( pHandleMap != NULL );
    return( (CImageList *)pHandleMap->FromHandle( hImageList ) );
}

CImageList * PASCAL CImageList::FromHandlePermanent( HIMAGELIST hImageList )
/**************************************************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHIMAGELIST;
    ASSERT( pHandleMap != NULL );
    return( (CImageList *)pHandleMap->LookupPermanent( hImageList ) );
}
