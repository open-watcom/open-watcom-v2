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
* Description:  Implementation of CEditView.
*
****************************************************************************/


#include "stdafx.h"
#include "edstate.h"

// Register FINDMSGSTRING message.
static const UINT _FindReplaceCmd = ::RegisterWindowMessage( FINDMSGSTRING );

// Instantiate the one and only _AFX_EDIT_STATE object.
CProcessLocal< _AFX_EDIT_STATE > _afxEditState;

IMPLEMENT_DYNAMIC( CEditView, CCtrlView )

BEGIN_MESSAGE_MAP( CEditView, CCtrlView )
    ON_COMMAND( ID_EDIT_CLEAR, OnEditClear )
    ON_COMMAND( ID_EDIT_COPY, OnEditCopy )
    ON_COMMAND( ID_EDIT_CUT, OnEditCut )
    ON_COMMAND( ID_EDIT_FIND, OnEditFind )
    ON_COMMAND( ID_EDIT_PASTE, OnEditPaste )
    ON_COMMAND( ID_EDIT_REPEAT, OnEditRepeat )
    ON_COMMAND( ID_EDIT_REPLACE, OnEditReplace )
    ON_COMMAND( ID_EDIT_SELECT_ALL, OnEditSelectAll )
    ON_COMMAND( ID_EDIT_UNDO, OnEditUndo )
    ON_UPDATE_COMMAND_UI( ID_EDIT_CLEAR, OnUpdateNeedSel )
    ON_UPDATE_COMMAND_UI( ID_EDIT_COPY, OnUpdateNeedSel )
    ON_UPDATE_COMMAND_UI( ID_EDIT_CUT, OnUpdateNeedSel )
    ON_UPDATE_COMMAND_UI( ID_EDIT_FIND, OnUpdateNeedText )
    ON_UPDATE_COMMAND_UI( ID_EDIT_PASTE, OnUpdateNeedClip )
    ON_UPDATE_COMMAND_UI( ID_EDIT_REPEAT, OnUpdateNeedFind )
    ON_UPDATE_COMMAND_UI( ID_EDIT_REPLACE, OnUpdateNeedText )
    ON_UPDATE_COMMAND_UI( ID_EDIT_SELECT_ALL, OnUpdateNeedText )
    ON_UPDATE_COMMAND_UI( ID_EDIT_UNDO, OnUpdateNeedUndo )
    ON_CONTROL_REFLECT_EX( EN_CHANGE, OnEditChange )
    ON_REGISTERED_MESSAGE( _FindReplaceCmd, OnFindReplaceCmd )
END_MESSAGE_MAP()

const DWORD CEditView::dwStyleDefault =
    AFX_WS_DEFAULT_VIEW | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
    ES_MULTILINE | ES_NOHIDESEL;

CEditView::CEditView()
    : CCtrlView( _T("EDIT"), 0L )
/*******************************/
{
    m_nTabStops = 32;
    m_pShadowBuffer = NULL;
    m_nShadowSize = 0;
    m_hPrinterFont = NULL;
}

void CEditView::OnFindNext( LPCTSTR lpszFind, BOOL bNext, BOOL bCase )
/********************************************************************/
{
    _AFX_EDIT_STATE *pState = _afxEditState.GetData();
    ASSERT( pState != NULL );
    
    pState->strFind = lpszFind;
    pState->bNext = bNext;
    pState->bCase = bCase;
    
    if( !FindText( lpszFind, bNext, bCase ) ) {
        OnTextNotFound( lpszFind );
    }
}

void CEditView::OnReplaceAll( LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase )
/*******************************************************************************/
{
    _AFX_EDIT_STATE *pState = _afxEditState.GetData();
    ASSERT( pState != NULL );
    
    pState->strFind = lpszFind;
    pState->strReplace = lpszReplace;
    pState->bCase = bCase;
    
    if( !FindText( lpszFind, TRUE, bCase ) ) {
        OnTextNotFound( lpszFind );
    } else {
        ::SendMessage( m_hWnd, EM_REPLACESEL, TRUE, (LPARAM)lpszReplace );
        while( FindText( lpszFind, TRUE, bCase ) ) {
            ::SendMessage( m_hWnd, EM_REPLACESEL, TRUE, (LPARAM)lpszReplace );
        }
    }
}

void CEditView::OnReplaceSel( LPCTSTR lpszFind, BOOL bNext, BOOL bCase,
                              LPCTSTR lpszReplace )
/*************************************************/
{
    _AFX_EDIT_STATE *pState = _afxEditState.GetData();
    ASSERT( pState != NULL );
    
    pState->strFind = lpszFind;
    pState->bNext = bNext;
    pState->bCase = bCase;
    pState->strReplace = lpszReplace;
    
    CString strSelection;
    GetSelectedText( strSelection );
    if( (bCase && strSelection.Compare( lpszFind ) != 0) ||
        (!bCase && strSelection.Compare( lpszFind ) != 0) ) {
        if( !FindText( lpszFind, bNext, bCase ) ) {
            OnTextNotFound( lpszFind );
            return;
        }
    }
    ::SendMessage( m_hWnd, EM_REPLACESEL, TRUE, (LPARAM)lpszReplace );
    FindText( lpszFind, bNext, bCase );
}

void CEditView::OnTextNotFound( LPCTSTR lpszFind )
/************************************************/
{
    UNUSED_ALWAYS( lpszFind );
    ::MessageBeep( MB_OK );
}

void CEditView::OnBeginPrinting( CDC *pDC, CPrintInfo *pInfo )
/************************************************************/
{
    CView::OnBeginPrinting( pDC, pInfo );
    m_aPageStart.Add( 0 );
}

void CEditView::OnPrepareDC( CDC *pDC, CPrintInfo *pInfo )
/********************************************************/
{
    UNUSED_ALWAYS( pDC );
    if( pInfo != NULL && pInfo->m_nCurPage > m_aPageStart.GetCount() ) {
        pInfo->m_bContinuePrinting = FALSE;
    }
}

BOOL CEditView::OnPreparePrinting( CPrintInfo *pInfo )
/****************************************************/
{
    return( DoPreparePrinting( pInfo ) );
}

void CEditView::OnPrint( CDC *pDC, CPrintInfo *pInfo )
/****************************************************/
{
    ASSERT( pDC != NULL );
    ASSERT( pInfo != NULL );
    HFONT hOldFont = NULL;
    if( m_hPrinterFont != NULL ) {
        hOldFont = (HFONT)::SelectObject( pDC->m_hDC, m_hPrinterFont );
    }
    int n = m_aPageStart.GetAt( pInfo->m_nCurPage - 1 );
    n = PrintInsideRect( pDC, pInfo->m_rectDraw, n, GetBufferLength() );
    if( pInfo->m_nCurPage == m_aPageStart.GetCount() && n < GetBufferLength() ) {
        m_aPageStart.Add( n );
    }
    if( hOldFont != NULL ) {
        ::SelectObject( pDC->m_hDC, hOldFont );
    }
}
    
BOOL CEditView::PreCreateWindow( CREATESTRUCT &cs )
/*************************************************/
{
    if( m_dwDefaultStyle == 0L ) {
        m_dwDefaultStyle = dwStyleDefault;
    }
    return( CCtrlView::PreCreateWindow( cs ) );
}

#ifdef _DEBUG

void CEditView::Dump( CDumpContext &dc ) const
/********************************************/
{
    CCtrlView::Dump( dc );

    dc << "m_nTabStops = " << m_nTabStops << "\n";
    dc << "m_pShadowBuffer = " << (void *)m_pShadowBuffer << "\n";
    dc << "m_nShadowSize = " << m_nShadowSize << "\n";
    dc << "m_hPrinterFont = " << m_hPrinterFont << "\n";
}

#endif // _DEBUG

BOOL CEditView::FindText( LPCTSTR lpszFind, BOOL bNext, BOOL bCase )
/******************************************************************/
{
    if( ::GetWindowTextLength( m_hWnd ) == 0 ) {
        return( FALSE );
    }
    
    DWORD   dwSelStart;
    DWORD   dwSelEnd;
    ::SendMessage( m_hWnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );

    UINT    nBufferLength = GetBufferLength();
    LPCTSTR lpszBuffer = LockBuffer();
    UINT    nFindLength = _tcslen( lpszFind );
    if( bNext ) {
        int i = dwSelEnd;
        while( i >= dwSelEnd || i < dwSelEnd - 1 ) {
            if( (bCase && _tcsncmp( lpszBuffer + i, lpszFind, nFindLength ) == 0) ||
                (!bCase && _tcsnicmp( lpszBuffer + i, lpszFind, nFindLength ) == 0) ) {
                ::SendMessage( m_hWnd, EM_SETSEL, i, i + nFindLength );
                return( TRUE );
            }
            i++;
            if( i >= nBufferLength ) {
                i = 0;
            }
        }
    } else {
        int i = dwSelStart - 1;
        while( i <= dwSelStart - 1 || i > dwSelStart ) {
            if( (bCase && _tcsncmp( lpszBuffer + i, lpszFind, nFindLength ) == 0) ||
                (!bCase && _tcsnicmp( lpszBuffer + i, lpszFind, nFindLength ) == 0) ) {
                ::SendMessage( m_hWnd, EM_SETSEL, i, i + nFindLength );
                return( TRUE );
            }
            i--;
            if( i < 0 ) {
                i = nBufferLength - nFindLength;
            }
        }
    }
    return( FALSE );
}

UINT CEditView::GetBufferLength() const
/*************************************/
{
    return( ::GetWindowTextLength( m_hWnd ) );
}

void CEditView::GetSelectedText( CString &strResult ) const
/*********************************************************/
{
    DWORD   dwSelStart;
    DWORD   dwSelEnd;
    ::SendMessage( m_hWnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );
    if( dwSelEnd > dwSelStart ) {
        LPCTSTR lpszBuffer = LockBuffer();
        strResult.SetString( lpszBuffer + dwSelStart, dwSelEnd - dwSelStart );
        UnlockBuffer();
    } else {
        strResult.Empty();
    }
}

LPCTSTR CEditView::LockBuffer() const
/***********************************/
{
    // This function can be called on a constant object even though it modifies
    // private member variables.
    CEditView *pThis = (CEditView *)this;
    int nLength = ::GetWindowTextLength( m_hWnd ) + 1;
    if( m_nShadowSize < nLength ) {
        if( m_pShadowBuffer != NULL ) {
            delete [] m_pShadowBuffer;
        }
        pThis->m_nShadowSize = nLength;
        pThis->m_pShadowBuffer = new TCHAR[m_nShadowSize];
    }
    ASSERT( m_pShadowBuffer != NULL );
    ::GetWindowText( m_hWnd, pThis->m_pShadowBuffer, m_nShadowSize );
    return( m_pShadowBuffer );
}

UINT CEditView::PrintInsideRect( CDC *pDC, RECT &rectLayout, UINT nIndexStart,
                                 UINT nIndexStop )
/************************************************/
{
    ASSERT( pDC != NULL );

    LPCTSTR lpszBuffer = LockBuffer();
    
    TEXTMETRIC tm;
    pDC->GetTextMetrics( &tm );

    int nWidth = rectLayout.right - rectLayout.left;
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    int y = rectLayout.top;
    int nCurrentIndex = nIndexStart;
    while( y + nCharHeight < rectLayout.bottom && nCurrentIndex <= nIndexStop ) {
        if( lpszBuffer[nCurrentIndex] == _T('\r') ) {
            nCurrentIndex++;
            if( lpszBuffer[nCurrentIndex] == _T('\n') ) {
                nCurrentIndex++;
            }
            y += nCharHeight;
            continue;
        } else if( lpszBuffer[nCurrentIndex] == _T('\0') ) {
            nCurrentIndex++;
            continue;
        }
        
        int nLength = 0;
        int nLineWidth = 0;
        int nLastSpace = 0;
        while( nLineWidth < nWidth && nCurrentIndex + nLength <= nIndexStop &&
               lpszBuffer[nCurrentIndex + nLength] != _T('\r') &&
               lpszBuffer[nCurrentIndex + nLength] != _T('\n') &&
               lpszBuffer[nCurrentIndex + nLength] != _T('\0') ) {
            nLength++;
            nLineWidth = pDC->GetTabbedTextExtent( lpszBuffer + nCurrentIndex, nLength,
                                                   1, &m_nTabStops ).cx;
            if( _istspace( lpszBuffer[nCurrentIndex + nLength] ) ) {
                nLastSpace = nLength;
            }
        }
        if( !(GetStyle() & ES_AUTOHSCROLL) && nLastSpace > 0 ) {
            nLength = nLastSpace + 1;
        } else if( nLineWidth >= nWidth ) {
            nLength--;
        }
        pDC->TabbedTextOut( rectLayout.left, y, lpszBuffer + nCurrentIndex, nLength, 1,
                            &m_nTabStops, rectLayout.left );
        nCurrentIndex += nLength;
        if( GetStyle() & ES_AUTOHSCROLL ) {
            while( lpszBuffer[nCurrentIndex] != _T('\r') &&
                   lpszBuffer[nCurrentIndex] != _T('\n') ) {
                nCurrentIndex++;
            }
        }
        if( lpszBuffer[nCurrentIndex] == _T('\r') ) {
            nCurrentIndex++;
            if( lpszBuffer[nCurrentIndex] == _T('\n') ) {
                nCurrentIndex++;
            }
        } else if( lpszBuffer[nCurrentIndex] == _T('\n') ) {
            nCurrentIndex++;
        }
        y += nCharHeight;
    }

    UnlockBuffer();
    return( nCurrentIndex );
}

void CEditView::SerializeRaw( CArchive &ar )
/******************************************/
{
    if( ar.IsStoring() ) {
        int     nLength = ::GetWindowTextLength( m_hWnd );
        LPCTSTR lpszBuffer = LockBuffer();
        if( lpszBuffer != NULL ) {
            ar.Write( lpszBuffer, nLength );
        }
    } else {
        CFile *pFile = ar.GetFile();
        ASSERT( pFile != NULL );

        ULONGLONG   nLength = pFile->GetLength();
        LRESULT     lLimit = ::SendMessage( m_hWnd, EM_GETLIMITTEXT, 0, 0L );
        if( nLength > lLimit ) {
            throw new CArchiveException( CArchiveException::badIndex );
        } else if( nLength == 0LL ) {
            ::SetWindowText( m_hWnd, NULL );
        } else {
            LPTSTR lpszBuffer = new TCHAR[nLength + 1];
            ar.Read( lpszBuffer, (UINT)nLength * sizeof( TCHAR ) );
            lpszBuffer[nLength] = _T('\0');
            ::SetWindowText( m_hWnd, lpszBuffer );
        }
    }
}

void CEditView::SetTabStops( int nTabStops )
/******************************************/
{
    m_nTabStops = nTabStops;
    ::SendMessage( m_hWnd, EM_SETTABSTOPS, 1, (LPARAM)&nTabStops );
}

void CEditView::OnEditClear()
/***************************/
{
    ::SendMessage( m_hWnd, WM_CLEAR, 0, 0L );
}

void CEditView::OnEditCopy()
/**************************/
{
    ::SendMessage( m_hWnd, WM_COPY, 0, 0L );
}

void CEditView::OnEditCut()
/*************************/
{
    ::SendMessage( m_hWnd, WM_CUT, 0, 0L );
}

void CEditView::OnEditFind()
/**************************/
{
    CFindReplaceDialog *&pDlg = _afxEditState->pFindReplaceDlg;
    if( pDlg != NULL && pDlg->GetOwner() == this ) {
        if( _afxEditState->bFindOnly ) {
            pDlg->SetActiveWindow();
            return;
        }
        pDlg->DestroyWindow();
        delete pDlg;
    }

    DWORD dwFlags = FR_HIDEWHOLEWORD;
    if( _afxEditState->bNext ) {
        dwFlags |= FR_DOWN;
    }
    if( _afxEditState->bCase ) {
        dwFlags |= FR_MATCHCASE;
    }
    _afxEditState->bFindOnly = TRUE;
    pDlg = new CFindReplaceDialog;
    pDlg->Create( TRUE, _afxEditState->strFind, NULL, dwFlags, this );
}

void CEditView::OnEditPaste()
/***************************/
{
    ::SendMessage( m_hWnd, WM_PASTE, 0, 0L );
}

void CEditView::OnEditRepeat()
/****************************/
{
    if( !FindText( _afxEditState->strFind, _afxEditState->bNext,
                   _afxEditState->bCase ) ) {
        OnTextNotFound( _afxEditState->strFind );
    }
}

void CEditView::OnEditReplace()
/*****************************/
{
    _AFX_EDIT_STATE *pState = _afxEditState.GetData();
    ASSERT( pState != NULL );
    
    CFindReplaceDialog *&pDlg = pState->pFindReplaceDlg;
    if( pDlg != NULL && pDlg->GetOwner() == this ) {
        if( !pState->bFindOnly ) {
            pDlg->SetActiveWindow();
            return;
        }
        pDlg->DestroyWindow();
        delete pDlg;
    }

    DWORD dwFlags = FR_HIDEWHOLEWORD;
    if( pState->bNext ) {
        dwFlags |= FR_DOWN;
    }
    if( pState->bCase ) {
        dwFlags |= FR_MATCHCASE;
    }
    pState->bFindOnly = FALSE;
    pDlg = new CFindReplaceDialog;
    pDlg->Create( FALSE, pState->strFind, pState->strReplace, dwFlags,
                  this );
}

void CEditView::OnEditSelectAll()
/*******************************/
{
    int nLength = ::GetWindowTextLength( m_hWnd );
    ::SendMessage( m_hWnd, EM_SETSEL, 0, nLength );
}

void CEditView::OnEditUndo()
/**************************/
{
    ::SendMessage( m_hWnd, EM_UNDO, 0, 0L );
}

void CEditView::OnUpdateNeedClip( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );
    pCmdUI->Enable( ::IsClipboardFormatAvailable( CF_TEXT ) );
}

void CEditView::OnUpdateNeedFind( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );
    pCmdUI->Enable( !_afxEditState->strFind.IsEmpty() );
}

void CEditView::OnUpdateNeedSel( CCmdUI *pCmdUI )
/***********************************************/
{
    DWORD   dwSelStart;
    DWORD   dwSelEnd;
    ::SendMessage( m_hWnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );
    ASSERT( pCmdUI != NULL );
    pCmdUI->Enable( dwSelStart != dwSelEnd );
}

void CEditView::OnUpdateNeedText( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );
    pCmdUI->Enable( ::GetWindowTextLength( m_hWnd ) > 0 );
}

void CEditView::OnUpdateNeedUndo( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );
    pCmdUI->Enable( ::SendMessage( m_hWnd, EM_CANUNDO, 0, 0L ) );
}

BOOL CEditView::OnEditChange()
/****************************/
{
    CDocument *pDoc = GetDocument();
    ASSERT( pDoc != NULL );
    pDoc->SetModifiedFlag();

    return( FALSE );
}

LRESULT CEditView::OnFindReplaceCmd( WPARAM wParam, LPARAM lParam )
/*****************************************************************/
{
    UNUSED_ALWAYS( wParam );

    CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier( lParam );
    ASSERT( pDlg == _afxEditState->pFindReplaceDlg );
    if( pDlg->FindNext() ) {
        OnFindNext( pDlg->GetFindString(), pDlg->SearchDown(), pDlg->MatchCase() );
    } else if( pDlg->ReplaceCurrent() ) {
        OnReplaceSel( pDlg->GetFindString(), pDlg->SearchDown(), pDlg->MatchCase(),
                      pDlg->GetReplaceString() );
    } else if( pDlg->ReplaceAll() ) {
        OnReplaceAll( pDlg->GetFindString(), pDlg->GetReplaceString(),
                      pDlg->MatchCase() );
    } else if( pDlg->IsTerminating() ) {
        _afxEditState->pFindReplaceDlg = NULL;
    }
    return( 0L );
}
