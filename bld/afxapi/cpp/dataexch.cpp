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
* Description:  Implementation of CDataExchange, DDX_*, and DDV_*.
*
****************************************************************************/


#include "stdafx.h"

CDataExchange::CDataExchange( CWnd *pDlgWnd, BOOL bSaveAndValidate )
/******************************************************************/
{
    ASSERT( pDlgWnd != NULL );
    m_pDlgWnd = pDlgWnd;
    m_bSaveAndValidate = bSaveAndValidate;
}

void CDataExchange::Fail()
/************************/
{
    HWND hWndCtrl;
    m_pDlgWnd->GetDlgItem( m_idLastControl, &hWndCtrl );
    if( hWndCtrl != NULL ) {
        ::SetFocus( hWndCtrl );
        if( m_bEditLastControl ) {
            ::SendMessage( hWndCtrl, EM_SETSEL, 0, -1L );
        }
    }
    throw new CUserException;
}

HWND CDataExchange::PrepareCtrl( int nIDC )
/*****************************************/
{
    HWND hWndCtrl;
    m_pDlgWnd->GetDlgItem( nIDC, &hWndCtrl );
    if( hWndCtrl == NULL ) {
        throw new CUserException;
    }
    m_idLastControl = nIDC;
    m_bEditLastControl = FALSE;
    return( hWndCtrl );
}

HWND CDataExchange::PrepareEditCtrl( int nIDC )
/*********************************************/
{
    HWND hWndCtrl;
    m_pDlgWnd->GetDlgItem( nIDC, &hWndCtrl );
    if( hWndCtrl == NULL ) {
        throw new CUserException;
    }
    m_idLastControl = nIDC;
    m_bEditLastControl = TRUE;
    return( hWndCtrl );
}

void AFXAPI DDV_MaxChars( CDataExchange *pDX, const CString &value, int nChars )
/******************************************************************************/
{
    if( pDX->m_bSaveAndValidate ) {
        if( value.GetLength() > nChars ) {
            CString strMessage;
            TCHAR   szChars[12];
            _stprintf( szChars, _T("%ld"), nChars );
            strMessage.FormatMessage( AFX_IDP_PARSE_STRING_SIZE, szChars );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxByte( CDataExchange *pDX, BYTE value, BYTE minVal, BYTE maxVal )
/************************************************************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[4];
            TCHAR   szMax[4];
            _stprintf( szMin, _T("%hhu"), minVal );
            _stprintf( szMax, _T("%hhu"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxDateTime( CDataExchange *pDX, CTime &refValue,
                                const CTime *refMinRange, const CTime *refMaxRange )
/**********************************************************************************/
{
    ASSERT( refMinRange == NULL || refMaxRange == NULL || *refMinRange <= *refMaxRange );
    ASSERT( pDX->m_pDlgWnd != NULL );

    if( !pDX->m_bSaveAndValidate ) {
        if( *refMinRange != NULL && refValue < *refMinRange ) {
            return;
        }
        if( *refMaxRange != NULL && refValue > *refMaxRange ) {
            return;
        }
    }
    
    HWND hWnd = ::GetDlgItem( pDX->m_pDlgWnd->m_hWnd, pDX->m_idLastControl );
    ASSERT( hWnd != NULL );

    SYSTEMTIME  st[2];
    UINT        nFlags = 0;
    if( refMinRange != NULL ) {
        refMinRange->GetAsSystemTime( st[0] );
        nFlags |= GDTR_MIN;
    }
    if( refMaxRange != NULL ) {
        refMaxRange->GetAsSystemTime( st[1] );
        nFlags |= GDTR_MAX;
    }
    ::SendMessage( hWnd, DTM_SETRANGE, nFlags, (LPARAM)&st );
}

void AFXAPI DDV_MinMaxDouble( CDataExchange *pDX, const double &value, double minVal,
                              double maxVal )
/*******************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[64];
            TCHAR   szMax[64];
            _stprintf( szMin, _T("%g"), minVal );
            _stprintf( szMax, _T("%g"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_REAL_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxDWord( CDataExchange *pDX, const DWORD &value, DWORD minVal,
                             DWORD maxVal )
/*****************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[11];
            TCHAR   szMax[11];
            _stprintf( szMin, _T("%lu"), minVal );
            _stprintf( szMax, _T("%lu"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxFloat( CDataExchange *pDX, float value, float minVal,
                             float maxVal )
/*****************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[64];
            TCHAR   szMax[64];
            _stprintf( szMin, _T("%hg"), minVal );
            _stprintf( szMax, _T("%hg"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_REAL_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxInt( CDataExchange *pDX, int value, int minVal, int maxVal )
/********************************************************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[12];
            TCHAR   szMax[12];
            _stprintf( szMin, _T("%ld"), minVal );
            _stprintf( szMax, _T("%ld"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxLong( CDataExchange *pDX, long value, long minVal, long maxVal )
/************************************************************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[12];
            TCHAR   szMax[12];
            _stprintf( szMin, _T("%ld"), minVal );
            _stprintf( szMax, _T("%ld"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxLongLong( CDataExchange *pDX, LONGLONG value, LONGLONG minVal,
                                LONGLONG maxVal )
/***********************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[22];
            TCHAR   szMax[22];
            _stprintf( szMin, _T("%I64d"), minVal );
            _stprintf( szMax, _T("%I64d"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxMonth( CDataExchange *pDX, CTime &refValue,
                             const CTime *refMinRange, const CTime *refMaxRange )
/*******************************************************************************/
{
    ASSERT( refMinRange == NULL || refMaxRange == NULL || *refMinRange <= *refMaxRange );
    ASSERT( pDX->m_pDlgWnd != NULL );

    if( !pDX->m_bSaveAndValidate ) {
        if( *refMinRange != NULL && refValue < *refMinRange ) {
            return;
        }
        if( *refMaxRange != NULL && refValue > *refMaxRange ) {
            return;
        }
    }
    
    HWND hWnd = ::GetDlgItem( pDX->m_pDlgWnd->m_hWnd, pDX->m_idLastControl );
    ASSERT( hWnd != NULL );

    SYSTEMTIME  st[2];
    UINT        nFlags = 0;
    if( refMinRange != NULL ) {
        refMinRange->GetAsSystemTime( st[0] );
        nFlags |= GDTR_MIN;
    }
    if( refMaxRange != NULL ) {
        refMaxRange->GetAsSystemTime( st[1] );
        nFlags |= GDTR_MAX;
    }
    ::SendMessage( hWnd, MCM_SETRANGE, nFlags, (LPARAM)&st );
}

void AFXAPI DDV_MinMaxShort( CDataExchange *pDX, short value, short minVal,
                             short maxVal )
/*****************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[7];
            TCHAR   szMax[7];
            _stprintf( szMin, _T("%hd"), minVal );
            _stprintf( szMax, _T("%hd"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxSlider( CDataExchange *pDX, DWORD value,
                              DWORD minVal, DWORD maxVal )
/********************************************************/
{
    ASSERT( minVal <= maxVal );
    ASSERT( pDX->m_pDlgWnd != NULL );

    if( !pDX->m_bSaveAndValidate && (value < minVal || value > maxVal) ) {
        return;
    }
    
    HWND hWnd = ::GetDlgItem( pDX->m_pDlgWnd->m_hWnd, pDX->m_idLastControl );
    ASSERT( hWnd != NULL );

    ::SendMessage( hWnd, TBM_SETRANGEMIN, FALSE, minVal );
    ::SendMessage( hWnd, TBM_SETRANGEMAX, TRUE, maxVal );
}

void AFXAPI DDV_MinMaxUInt( CDataExchange *pDX, UINT value, UINT minVal, UINT maxVal )
/************************************************************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[11];
            TCHAR   szMax[11];
            _stprintf( szMin, _T("%lu"), minVal );
            _stprintf( szMax, _T("%lu"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxULongLong( CDataExchange *pDX, ULONGLONG value, ULONGLONG minVal,
                                 ULONGLONG maxVal )
/*************************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[21];
            TCHAR   szMax[21];
            _stprintf( szMin, _T("%I64u"), minVal );
            _stprintf( szMax, _T("%I64u"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDV_MinMaxUnsigned( CDataExchange *pDX, unsigned value, unsigned minVal,
                                unsigned maxVal )
/***********************************************/
{
    ASSERT( minVal <= maxVal );
    if( pDX->m_bSaveAndValidate ) {
        if( value < minVal || value > maxVal ) {
            CString strMessage;
            TCHAR   szMin[11];
            TCHAR   szMax[11];
            _stprintf( szMin, _T("%lu"), minVal );
            _stprintf( szMax, _T("%lu"), maxVal );
            strMessage.FormatMessage( AFX_IDP_PARSE_INT_RANGE, szMin, szMax );
            AfxMessageBox( strMessage );
            pDX->Fail();
        }
    }
}

void AFXAPI DDX_CBIndex( CDataExchange *pDX, int nIDC, int &index )
/*****************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        index = (int)::SendMessage( hWnd, CB_GETCURSEL, 0, 0L );
    } else {
        ::SendMessage( hWnd, CB_SETCURSEL, index, 0L );
    }
}

void AFXAPI DDX_CBString( CDataExchange *pDX, int nIDC, CString &value )
/**********************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        LPTSTR lpszBuffer = value.GetBuffer( 256 );
        ::GetWindowText( hWnd, lpszBuffer, 255 );
        value.ReleaseBuffer();
    } else {
        int index = (int)::SendMessage( hWnd, CB_FINDSTRING, -1,
                                        (LPARAM)(LPCTSTR)value );
        if( index >= 0 ) {
            ::SendMessage( hWnd, CB_SETCURSEL, index, 0L );
        } else {
            ::SetWindowText( hWnd, (LPCTSTR)value );
        }
    }
}

void AFXAPI DDX_CBStringExact( CDataExchange *pDX, int nIDC, CString &value )
/***************************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        LPTSTR lpszBuffer = value.GetBuffer( 256 );
        ::GetWindowText( hWnd, lpszBuffer, 255 );
        value.ReleaseBuffer();
    } else {
        int index = (int)::SendMessage( hWnd, CB_FINDSTRINGEXACT, -1,
                                        (LPARAM)(LPCTSTR) value );
        if( index >= 0 ) {
            ::SendMessage( hWnd, CB_SETCURSEL, index, 0L );
        } else {
            ::SetWindowText( hWnd, (LPCTSTR)value );
        }
    }
}

void AFXAPI DDX_Check( CDataExchange *pDX, int nIDC, int &value )
/***************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        value = (int)::SendMessage( hWnd, BM_GETCHECK, 0, 0L );
    } else {
        ::SendMessage( hWnd, BM_SETCHECK, value, 0L );
    }
}

void AFXAPI DDX_Control( CDataExchange *pDX, int nIDC, CWnd &rControl )
/*********************************************************************/
{
    if( rControl.m_hWnd == NULL ) {
        HWND hWnd = pDX->PrepareCtrl( nIDC );
        rControl.SubclassWindow( hWnd );
    }
}

void AFXAPI DDX_DateTimeCtrl( CDataExchange *pDX, int nIDC, CTime &value )
/************************************************************************/
{
    SYSTEMTIME  st;
    HWND        hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        ::SendMessage( hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&st );
        value = CTime( st ).GetTime();
    } else {
        value.GetAsSystemTime( st );
        ::SendMessage( hWnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&st );
    }
}

void AFXAPI DDX_IPAddress( CDataExchange *pDX, int nIDC, DWORD &value )
/*********************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        ::SendMessage( hWnd, IPM_GETADDRESS, 0, (LPARAM)&value );
    } else {
        ::SendMessage( hWnd, IPM_SETADDRESS, 0, value );
    }
}

void AFXAPI DDX_LBIndex( CDataExchange *pDX, int nIDC, int &index )
/*****************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        index = (int)::SendMessage( hWnd, LB_GETCURSEL, 0, 0L );
    } else {
        ::SendMessage( hWnd, LB_SETCURSEL, index, 0L );
    }
}

void AFXAPI DDX_LBString( CDataExchange *pDX, int nIDC, CString &value )
/**********************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        int index = (int)::SendMessage( hWnd, LB_GETCURSEL, 0, 0L );
        if( index >= 0 ) {
            int nLength = (int)::SendMessage( hWnd, LB_GETTEXTLEN, index, 0L );
            LPTSTR lpszBuffer = value.GetBuffer( nLength + 1 );
            ::SendMessage( hWnd, LB_GETTEXT, index, (LPARAM)lpszBuffer );
            value.ReleaseBuffer();
        } else {
            value.Empty();
        }
    } else {
        int index = (int)::SendMessage( hWnd, LB_FINDSTRING, -1,
                                        (LPARAM)(LPCTSTR)value );
        ::SendMessage( hWnd, LB_SETCURSEL, index, 0L );
    }
}

void AFXAPI DDX_LBStringExact( CDataExchange *pDX, int nIDC, CString &value )
/***************************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        int index = (int)::SendMessage( hWnd, LB_GETCURSEL, 0, 0L );
        if( index >= 0 ) {
            int nLength = (int)::SendMessage( hWnd, LB_GETTEXTLEN, index, 0L );
            LPTSTR lpszBuffer = value.GetBuffer( nLength + 1 );
            ::SendMessage( hWnd, LB_GETTEXT, index, (LPARAM)lpszBuffer );
            value.ReleaseBuffer();
        } else {
            value.Empty();
        }
    } else {
        int index = (int)::SendMessage( hWnd, LB_FINDSTRINGEXACT, -1,
                                        (LPARAM)(LPCTSTR)value );
        ::SendMessage( hWnd, LB_SETCURSEL, index, 0L );
    }
}

void AFXAPI DDX_MonthCalCtrl( CDataExchange *pDX, int nIDC, CTime &value )
/************************************************************************/
{
    SYSTEMTIME  st;
    HWND        hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        ::SendMessage( hWnd, MCM_GETCURSEL, 0, (LPARAM)&st );
        value = CTime( st ).GetTime();
    } else {
        value.GetAsSystemTime( st );
        ::SendMessage( hWnd, MCM_SETCURSEL, 0, (LPARAM)&st );
    }
}

void AFXAPI DDX_Radio( CDataExchange *pDX, int nIDC, int &value )
/***************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        int index = 0;
        for( ;; ) {
            if( ::SendMessage( hWnd, WM_GETDLGCODE, 0, 0L ) & DLGC_RADIOBUTTON ) {
                if( ::SendMessage( hWnd, BM_GETCHECK, 0, 0L ) == BST_CHECKED ) {
                    value = index;
                    break;
                }
                index++;
            }
            hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
            if( hWnd == NULL || ::GetWindowLong( hWnd, GWL_STYLE ) & WS_GROUP ) {
                value = -1;
                break;
            }
        }
    } else {
        int index = 0;
        for( ;; ) {
            if( ::SendMessage( hWnd, WM_GETDLGCODE, 0, 0L ) & DLGC_RADIOBUTTON ) {
                if( index == value ) {
                    break;
                }
                index++;
            }
            hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
            if( hWnd == NULL ) {
                break;
            }
            if( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_GROUP ) {
                hWnd = NULL;
                break;
            }
        }
        if( hWnd != NULL ) {
            ::SendMessage( hWnd, BM_SETCHECK, BST_CHECKED, 0L );
        }
    }
}

void AFXAPI DDX_Scroll( CDataExchange *pDX, int nIDC, int &value )
/****************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        value = ::GetScrollPos( hWnd, SB_CTL );
    } else {
        ::SetScrollPos( hWnd, SB_CTL, value, TRUE );
    }
}

void AFXAPI DDX_Slider( CDataExchange *pDX, int nIDC, int &value )
/****************************************************************/
{
    HWND hWnd = pDX->PrepareCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        value = (int)::SendMessage( hWnd, TBM_GETPOS, 0, 0L );
    } else {
        ::SendMessage( hWnd, TBM_SETPOS, TRUE, value );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, BYTE &value )
/***************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%hhu"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_BYTE );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[4];
        _stprintf( szBuff, _T("%hhu"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, short &value )
/****************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%hd"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_INT );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[7];
        _stprintf( szBuff, _T("%hd"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, int &value )
/**************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%ld"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_INT );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[12];
        _stprintf( szBuff, _T("%ld"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, UINT &value )
/***************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%lu"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_UINT );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[11];
        _stprintf( szBuff, _T("%lu"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, long &value )
/***************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%ld"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_INT );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[12];
        _stprintf( szBuff, _T("%ld"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, DWORD &value )
/****************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%lu"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_UINT );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[11];
        _stprintf( szBuff, _T("%lu"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, CString &value )
/******************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        int nLength = ::GetWindowTextLength( hWnd );
        LPTSTR lpszBuff = value.GetBuffer( nLength + 1 );
        ::GetWindowText( hWnd, lpszBuff, nLength + 1 );
        value.ReleaseBuffer();
    } else {
        ::SetWindowText( hWnd, (LPCTSTR)value );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, float &value )
/****************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%hg"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_REAL );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[11];
        _stprintf( szBuff, _T("%hg"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}

void AFXAPI DDX_Text( CDataExchange *pDX, int nIDC, double &value )
/*****************************************************************/
{
    HWND hWnd = pDX->PrepareEditCtrl( nIDC );
    if( pDX->m_bSaveAndValidate ) {
        TCHAR szBuff[256];
        ::GetWindowText( hWnd, szBuff, 255 );
        if( _stscanf( szBuff, _T("%g"), &value ) != 1 ) {
            AfxMessageBox( AFX_IDP_PARSE_REAL );
            pDX->Fail();
        }
    } else {
        TCHAR szBuff[11];
        _stprintf( szBuff, _T("%g"), value );
        ::SetWindowText( hWnd, szBuff );
    }
}
