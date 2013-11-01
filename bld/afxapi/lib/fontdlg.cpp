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
* Description:  Implementation of CFontDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

IMPLEMENT_DYNAMIC( CFontDialog, CCommonDialog )

CFontDialog::CFontDialog( LPLOGFONT lplfInitial, DWORD dwFlags, CDC *pdcPrinter,
                          CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    m_cf.lStructSize = sizeof( CHOOSEFONT );
    m_cf.hwndOwner = pParentWnd->GetSafeHwnd();
    m_cf.hDC = pdcPrinter->GetSafeHdc();
    if( lplfInitial != NULL ) {
        m_cf.lpLogFont = lplfInitial;
        dwFlags |= CF_INITTOLOGFONTSTRUCT;
    } else {
        m_cf.lpLogFont = &m_lf;
    }
    m_cf.iPointSize = 0;
    m_cf.Flags = dwFlags | CF_ENABLEHOOK;
    m_cf.rgbColors = 0L;
    m_cf.lCustData = 0L;
    m_cf.lpfnHook = AfxCommDlgProc;
    m_cf.lpTemplateName = NULL;
    m_cf.hInstance = AfxGetResourceHandle();
    m_cf.lpszStyle = m_szStyleName;
    m_cf.nFontType = 0;
    m_cf.nSizeMin = 0;
    m_cf.nSizeMax = 0;
    m_szStyleName[0] = _T('\0');
}

CFontDialog::CFontDialog( const CHARFORMAT &charformat, DWORD dwFlags, CDC *pdcPrinter,
                          CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    m_cf.lStructSize = sizeof( CHOOSEFONT );
    m_cf.hwndOwner = pParentWnd->GetSafeHwnd();
    m_cf.hDC = pdcPrinter->GetSafeHdc();
    m_cf.lpLogFont = &m_lf;
    m_cf.iPointSize = 0;
    m_cf.Flags = dwFlags | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
    if( charformat.dwMask & CFM_COLOR ) {
        if( charformat.dwEffects & CFE_AUTOCOLOR ) {
            m_cf.rgbColors = ::GetSysColor( COLOR_WINDOWTEXT );
        } else {
            m_cf.rgbColors = charformat.crTextColor;
        }
    } else {
        m_cf.rgbColors = 0L;
    }
    m_cf.lCustData = 0L;
    m_cf.lpfnHook = AfxCommDlgProc;
    m_cf.lpTemplateName = NULL;
    m_cf.hInstance = AfxGetResourceHandle();
    m_cf.lpszStyle = m_szStyleName;
    m_cf.nFontType = 0;
    m_cf.nSizeMin = 0;
    m_cf.nSizeMax = 0;
    m_szStyleName[0] = _T('\0');

    m_lf.lfHeight = charformat.yHeight;
    m_lf.lfWidth = 0L;
    m_lf.lfEscapement = 0L;
    if( charformat.dwMask & CFM_BOLD ) {
        if( charformat.dwEffects & CFE_BOLD ) {
            m_lf.lfWeight = FW_BOLD;
        } else {
            m_lf.lfWeight = FW_REGULAR;
        }
    } else {
        m_lf.lfWeight = FW_DONTCARE;
    }
    if( (charformat.dwMask & CFM_ITALIC) && (charformat.dwEffects & CFE_ITALIC) ) {
        m_lf.lfItalic = TRUE;
    } else {
        m_lf.lfItalic = FALSE;
    }
    if( (charformat.dwMask & CFM_UNDERLINE) && (charformat.dwEffects & CFE_UNDERLINE) ) {
        m_lf.lfUnderline = TRUE;
    } else {
        m_lf.lfUnderline = FALSE;
    }
    if( (charformat.dwMask & CFM_STRIKEOUT) && (charformat.dwEffects & CFE_STRIKEOUT) ) {
        m_lf.lfStrikeOut = TRUE;
    } else {
        m_lf.lfStrikeOut = FALSE;
    }
    if( charformat.dwMask & CFM_CHARSET ) {
        m_lf.lfCharSet = charformat.bCharSet;
    } else {
        m_lf.lfCharSet = DEFAULT_CHARSET;
    }
    m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    m_lf.lfQuality = DEFAULT_QUALITY;
    m_lf.lfPitchAndFamily = charformat.bPitchAndFamily;
    if( charformat.dwMask & CFM_FACE ) {
        _tcscpy( m_lf.lfFaceName, charformat.szFaceName );
    } else {
        m_lf.lfFaceName[0] = _T('\0');
    }
}

INT_PTR CFontDialog::DoModal()
/****************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;
    return( ::ChooseFont( &m_cf ) ? IDOK : IDCANCEL );
}

#ifdef _DEBUG

void CFontDialog::AssertValid() const
/***********************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_cf.lStructSize == sizeof( CHOOSEFONT ) );
}

void CFontDialog::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_cf.lStructSize = " << m_cf.lStructSize << "\n";
    dc << "m_cf.hwndOwner = " << m_cf.hwndOwner << "\n";
    dc << "m_cf.hDC = " << m_cf.hDC << "\n";
    if( m_cf.lpLogFont == &m_lf ) {
        dc << "m_cf.lpLogFont = &m_lf\n";
    } else {
        dc << "m_cf.lpLogFont = " << m_cf.lpLogFont << "\n";
    }
    dc << "m_cf.iPointSize = " << m_cf.iPointSize << "\n";
    dc << "m_cf.Flags = ";
    dc.DumpAsHex( m_cf.Flags );
    dc << "\n";
    dc << "m_cf.rgbColors = ";
    dc.DumpAsHex( m_cf.rgbColors );
    dc << "\n";
    dc << "m_cf.lCustData = ";
    dc.DumpAsHex( m_cf.lCustData );
    dc << "\n";
    if( m_cf.lpfnHook == AfxCommDlgProc ) {
        dc << "m_cf.lpfnHook = AfxCommDlgProc\n";
    } else {
        dc << "m_cf.lpfnHook = " << m_cf.lpfnHook << "\n";
    }
    if( IS_INTRESOURCE( m_cf.lpTemplateName ) ) {
        dc << "m_cf.lpTemplateName = " << (UINT)m_cf.lpTemplateName << "\n";
    } else {
        dc << "m_cf.lpTemplateName = " << m_cf.lpTemplateName << "\n";
    }
    dc << "m_cf.hInstance = " << m_cf.hInstance << "\n";
    dc << "m_cf.lpszStyle = " << m_cf.lpszStyle << "\n";
    dc << "m_cf.nFontType = " << m_cf.nFontType << "\n";
    dc << "m_cf.nSizeMin = " << m_cf.nSizeMin << "\n";
    dc << "m_cf.nSizeMax = " << m_cf.nSizeMax << "\n";
}

#endif // _DEBUG

void CFontDialog::GetCharFormat( CHARFORMAT &cf ) const
/*****************************************************/
{
    ASSERT( m_cf.lpLogFont != NULL );
    cf.cbSize = sizeof( CHARFORMAT );
    cf.dwMask = 0L;
    cf.dwEffects = 0L;
    cf.bPitchAndFamily = m_cf.lpLogFont->lfPitchAndFamily;
    if( m_cf.Flags & CF_EFFECTS ) {
        cf.dwMask |= CFM_COLOR | CFM_UNDERLINE | CFM_STRIKEOUT;
        cf.crTextColor = m_cf.rgbColors;
        if( m_cf.lpLogFont->lfUnderline ) {
            cf.dwEffects |= CFE_UNDERLINE;
        }
        if( m_cf.lpLogFont->lfStrikeOut ) {
            cf.dwEffects |= CFE_STRIKEOUT;
        }
    }
    if( !(m_cf.Flags & CF_NOSTYLESEL) ) {
        cf.dwMask |= CFM_BOLD | CFM_ITALIC;
        if( m_cf.lpLogFont->lfWeight == FW_BOLD ) {
            cf.dwEffects |= CFE_BOLD;
        }
        if( m_cf.lpLogFont->lfItalic ) {
            cf.dwEffects |= CFE_ITALIC;
        }
    }
    if( !(m_cf.Flags & CF_NOFACESEL) ) {
        cf.dwMask |= CFM_FACE;
        _tcscpy( cf.szFaceName, m_cf.lpLogFont->lfFaceName );
    }
    if( !(m_cf.Flags & CF_NOSIZESEL) ) {
        cf.dwMask |= CFM_SIZE;
        cf.yHeight = m_cf.lpLogFont->lfHeight;
    }
    if( !(m_cf.Flags & CF_NOSCRIPTSEL) ) {
        cf.dwMask |= CFM_CHARSET;
        cf.bCharSet = m_cf.lpLogFont->lfCharSet;
    }
}
