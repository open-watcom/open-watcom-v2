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
* Description:  Implementation of CDragListBox.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CDragListBox, CListBox )

CDragListBox::CDragListBox()
/**************************/
{
    m_nLast = -1;
}

BOOL CDragListBox::BeginDrag( CPoint pt )
/***************************************/
{
    ASSERT( m_nLast == -1 );
    int nItem = ItemFromPt( pt );
    if( nItem >= 0 ) {
        DrawInsert( nItem );
        return( TRUE );
    }
    return( FALSE );
}

void CDragListBox::CancelDrag( CPoint pt )
/****************************************/
{
    pt = pt;
    DrawInsert( -1 );
}

UINT CDragListBox::Dragging( CPoint pt )
/**************************************/
{
    int nItem = ItemFromPt( pt );
    DrawInsert( nItem );
    if( nItem >= 0 ) {
        return( DL_MOVECURSOR );
    } else {
        return( DL_STOPCURSOR );
    }
}

void CDragListBox::Dropped( int nSrcIndex, CPoint pt )
/****************************************************/
{
    int nDestIndex = ItemFromPt( pt );
    DrawInsert( -1 );
    if( nDestIndex >= 0 && nDestIndex != nSrcIndex) {
        CString     strText;
        DWORD_PTR   dwData;
        GetText( nSrcIndex, strText );
        dwData = GetItemData( nSrcIndex );
        DeleteString( nSrcIndex );
        if( nSrcIndex < nDestIndex ) {
            nDestIndex--;
        }
        InsertString( nDestIndex, strText );
        SetItemData( nDestIndex, dwData );
        ::UpdateWindow( m_hWnd );
    }
}

void CDragListBox::DrawInsert( int nItem )
/****************************************/
{
    if( m_nLast != nItem ) {
        ::DrawInsert( ::GetParent( m_hWnd ), m_hWnd, nItem );
        m_nLast = nItem;
    }
}

BOOL CDragListBox::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                                  LRESULT *pResult )
/**************************************************/
{
    if( message == m_nMsgDragList ) {
        DRAGLISTINFO *pInfo = (DRAGLISTINFO *)lParam;
        ASSERT( pInfo != NULL );
        switch( pInfo->uNotification ) {
        case DL_BEGINDRAG:
            *pResult = BeginDrag( pInfo->ptCursor );
            break;
        case DL_CANCELDRAG:
            CancelDrag( pInfo->ptCursor );
            break;
        case DL_DRAGGING:
            *pResult = Dragging( pInfo->ptCursor );
            break;
        case DL_DROPPED:
            Dropped( GetCurSel(), pInfo->ptCursor );
            break;
        }
        return( TRUE );
    }
    return( CListBox::OnChildNotify( message, wParam, lParam, pResult ) );
}

void CDragListBox::PreSubclassWindow()
/************************************/
{
    ::MakeDragList( m_hWnd );
}

#ifdef _DEBUG

void CDragListBox::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CListBox::Dump( dc );

    dc << "m_nLast = " << m_nLast << "\n";
}

#endif // _DEBUG
