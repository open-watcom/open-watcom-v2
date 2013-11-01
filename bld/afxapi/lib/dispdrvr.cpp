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
* Description:  Implementation of COleDispatchDriver.
*
****************************************************************************/


#include "stdafx.h"

COleDispatchDriver::COleDispatchDriver()
/**************************************/
{
    m_lpDispatch = NULL;
    m_bAutoRelease = TRUE;
}

COleDispatchDriver::COleDispatchDriver( LPDISPATCH lpDispatch, BOOL bAutoRelease )
/********************************************************************************/
{
    m_lpDispatch = lpDispatch;
    m_bAutoRelease = bAutoRelease;
}

COleDispatchDriver::COleDispatchDriver( const COleDispatchDriver &dispatchSrc )
/*****************************************************************************/
{
    m_lpDispatch = dispatchSrc.m_lpDispatch;
    m_bAutoRelease = TRUE;
    if( m_lpDispatch != NULL ) {
        m_lpDispatch->AddRef();
    }
}

COleDispatchDriver::~COleDispatchDriver()
/***************************************/
{
    if( m_bAutoRelease && m_lpDispatch != NULL ) {
        m_lpDispatch->Release();
    }
}

void COleDispatchDriver::AttachDispatch( LPDISPATCH lpDispatch, BOOL bAutoRelease )
/*********************************************************************************/
{
    if( m_bAutoRelease && m_lpDispatch != NULL ) {
        m_lpDispatch->Release();
    }
    m_lpDispatch = lpDispatch;
    m_bAutoRelease = bAutoRelease;
}

BOOL COleDispatchDriver::CreateDispatch( REFCLSID clsid, COleException *pError )
/******************************************************************************/
{
    ASSERT( m_lpDispatch == NULL );

    IUnknown    *punk = NULL;
    HRESULT     hr;

    hr = ::CoCreateInstance( clsid, NULL, CLSCTX_ALL, IID_IUnknown, (void **)&punk );
    if( FAILED( hr ) ) {
        hr = ::CoCreateInstance( clsid, NULL, CLSCTX_ALL & ~CLSCTX_REMOTE_SERVER,
                                 IID_IUnknown, (void **)&punk );
    }

    if( SUCCEEDED( hr ) && SUCCEEDED( ::OleRun( punk ) ) ) {
        if( SUCCEEDED( punk->QueryInterface( IID_IDispatch, (void **)&m_lpDispatch ) ) ) {
            if( m_lpDispatch != NULL ) {
                punk->Release();
                return( TRUE );
            }
        }
    }

    // If this point has been reached, creation has failed.
    if( punk != NULL ) {
        punk->Release();
    }
    if( pError != NULL ) {
        pError->m_sc = hr;
    }
    return( FALSE );
}

LPDISPATCH COleDispatchDriver::DetachDispatch()
/*********************************************/
{
    LPDISPATCH lpDispatch = m_lpDispatch;
    m_lpDispatch = NULL;
    return( lpDispatch );
}

void COleDispatchDriver::InvokeHelperV( DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
                                        void *pvRet, const BYTE *pbParamInfo,
                                        va_list argList )
/*******************************************************/
{
    int         cParams = strlen( (const char *)pbParamInfo );
    BOOL        *pBool = NULL;
    CPtrArray   arrBoolArgs;
#ifndef _UNICODE
    LPCSTR      lpsz1;
    LPOLESTR    lpsz2;
    int         nLen;
#endif
    DISPPARAMS  dp;
    EXCEPINFO   ei;
    VARIANTARG  varRet;
    HRESULT     hr;

    dp.rgvarg = new VARIANTARG[cParams];
    dp.rgdispidNamedArgs = NULL;
    dp.cArgs = cParams;
    dp.cNamedArgs = 0;

    for( int i = 0, j = cParams - 1; i < cParams; i++, j-- ) {
        dp.rgvarg[j].vt = pbParamInfo[i];
        if( dp.rgvarg[j].vt & 0x40 ) {
            // AFX uses a different flag from OLE to indicate if a parameter is passed by
            // reference in order to get each parameter's type into a single byte.
            // Therefore, the type value must be converted to use OLE's convention.
            dp.rgvarg[j].vt &= ~0x40;
            dp.rgvarg[j].vt |= VT_BYREF;
        }
        switch( dp.rgvarg[j].vt ) {
        case VT_I2:
            dp.rgvarg[j].iVal = va_arg( argList, SHORT );
            break;
        case VT_I4:
            dp.rgvarg[j].lVal = va_arg( argList, LONG );
            break;
        case VT_R4:
            dp.rgvarg[j].fltVal = va_arg( argList, FLOAT );
            break;
        case VT_R8:
            dp.rgvarg[j].dblVal = va_arg( argList, DOUBLE );
            break;
        case VT_CY:
            dp.rgvarg[j].cyVal = *va_arg( argList, CY * );
            break;
        case VT_DATE:
            dp.rgvarg[j].date = va_arg( argList, DATE );
            break;
        case VT_BSTR:
            dp.rgvarg[j].bstrVal = ::SysAllocString( va_arg( argList, LPCOLESTR ) );
            break;
        case VT_DISPATCH:
            dp.rgvarg[j].pdispVal = va_arg( argList, LPDISPATCH );
            break;
        case VT_ERROR:
            dp.rgvarg[j].scode = va_arg( argList, SCODE );
            break;
        case VT_BOOL:
            dp.rgvarg[j].boolVal = va_arg( argList, BOOL ) ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        case VT_VARIANT:
            memcpy( &dp.rgvarg[j], &va_arg( argList, VARIANTARG ), sizeof( VARIANTARG ) );
            break;
        case VT_UNKNOWN:
            dp.rgvarg[j].punkVal = va_arg( argList, LPUNKNOWN );
            break;
#ifndef _UNICODE
        case VT_BSTRA:
            lpsz1 = va_arg( argList, LPCSTR );
            nLen = strlen( lpsz1 ) + 1;
            lpsz2 = new OLECHAR[nLen];
            ::MultiByteToWideChar( CP_ACP, 0L, lpsz1, -1, lpsz2, nLen );
            dp.rgvarg[j].bstrVal = ::SysAllocString( lpsz2 );
            delete [] lpsz2;
            dp.rgvarg[j].vt = VT_BSTR;
            break;
#endif
        case VT_I1:
            dp.rgvarg[j].cVal = va_arg( argList, CHAR );
            break;
        case VT_UI1:
            dp.rgvarg[j].bVal = va_arg( argList, BYTE );
            break;
        case VT_UI2:
            dp.rgvarg[j].uiVal = va_arg( argList, USHORT );
            break;
        case VT_UI4:
            dp.rgvarg[j].ulVal = va_arg( argList, ULONG );
            break;
        case VT_I8:
            dp.rgvarg[j].llVal = va_arg( argList, LONGLONG );
            break;
        case VT_UI8:
            dp.rgvarg[j].ullVal = va_arg( argList, ULONGLONG );
            break;
        case VT_BYREF | VT_I2:
            dp.rgvarg[j].piVal = va_arg( argList, SHORT * );
            break;
        case VT_BYREF | VT_I4:
            dp.rgvarg[j].plVal = va_arg( argList, LONG * );
            break;
        case VT_BYREF | VT_R4:
            dp.rgvarg[j].pfltVal = va_arg( argList, FLOAT * );
            break;
        case VT_BYREF | VT_R8:
            dp.rgvarg[j].pdblVal = va_arg( argList, DOUBLE * );
            break;
        case VT_BYREF | VT_CY:
            dp.rgvarg[j].pcyVal = va_arg( argList, CY * );
            break;
        case VT_BYREF | VT_DATE:
            dp.rgvarg[j].pdate = va_arg( argList, DATE * );
            break;
        case VT_BYREF | VT_BSTR:
            dp.rgvarg[j].pbstrVal = va_arg( argList, BSTR * );
            break;
        case VT_BYREF | VT_DISPATCH:
            dp.rgvarg[j].ppdispVal = va_arg( argList, LPDISPATCH * );
            break;
        case VT_BYREF | VT_ERROR:
            dp.rgvarg[j].pscode = va_arg( argList, SCODE * );
            break;
        case VT_BYREF | VT_BOOL:
            pBool = va_arg( argList, BOOL * );
            ASSERT( pBool != NULL );
            arrBoolArgs.Add( pBool );
            dp.rgvarg[j].pboolVal = (VARIANT_BOOL *)pBool;
            *dp.rgvarg[j].pboolVal = *pBool ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        case VT_BYREF | VT_VARIANT:
            dp.rgvarg[j].pvarVal = va_arg( argList, VARIANTARG * );
            break;
        case VT_BYREF | VT_UNKNOWN:
            dp.rgvarg[j].ppunkVal = va_arg( argList, LPUNKNOWN * );
            break;
        case VT_BYREF | VT_I1:
            dp.rgvarg[j].pcVal = va_arg( argList, CHAR * );
            break;
        case VT_BYREF | VT_UI1:
            dp.rgvarg[j].pbVal = va_arg( argList, BYTE * );
            break;
        case VT_BYREF | VT_UI2:
            dp.rgvarg[j].puiVal = va_arg( argList, USHORT * );
            break;
        case VT_BYREF | VT_UI4:
            dp.rgvarg[j].pulVal = va_arg( argList, ULONG * );
            break;
        case VT_BYREF | VT_I8:
            dp.rgvarg[j].pllVal = va_arg( argList, LONGLONG * );
            break;
        case VT_BYREF | VT_UI8:
            dp.rgvarg[j].pullVal = va_arg( argList, ULONGLONG * );
            break;
        }
    }

    if( vtRet != VT_EMPTY ) {
        ::VariantInit( &varRet );
        hr = m_lpDispatch->Invoke( dwDispID, IID_NULL, 0L, wFlags, &dp, &varRet, &ei, NULL );
    } else {
        hr = m_lpDispatch->Invoke( dwDispID, IID_NULL, 0L, wFlags, &dp, NULL, &ei, NULL );
    }

    for( int i = 0; i < cParams; i++ ) {
        if( dp.rgvarg[i].vt == VT_BSTR ) {
            ASSERT( dp.rgvarg[i].bstrVal != NULL );
            ::SysFreeString( dp.rgvarg[i].bstrVal );
        }
    }

    if( FAILED( hr ) ) {
        if( hr == DISP_E_EXCEPTION ) {
            if( ei.pfnDeferredFillIn != NULL ) {
                ei.pfnDeferredFillIn( &ei );
            }

            COleDispatchException *pEx = new COleDispatchException( NULL, ei.dwHelpContext,
                                                                    ei.wCode );
            if( ei.bstrSource != NULL ) {
                if( ::SysStringLen( ei.bstrSource ) > 0 ) {
                    pEx->m_strSource = ei.bstrSource;
                }
                ::SysFreeString( ei.bstrSource );
            }
            if( ei.bstrDescription != NULL ) {
                if( ::SysStringLen( ei.bstrDescription ) > 0 ) {
                    pEx->m_strDescription = ei.bstrDescription;
                }
                ::SysFreeString( ei.bstrDescription );
            }
            if( ei.bstrHelpFile != NULL ) {
                if( ::SysStringLen( ei.bstrHelpFile ) > 0 ) {
                    pEx->m_strHelpFile = ei.bstrHelpFile;
                }
                ::SysFreeString( ei.bstrHelpFile );
            }
            throw pEx;
        } else {
            AfxThrowOleException( hr );
        }
    }

    for( int i = 0; i < arrBoolArgs.GetCount(); i++ ) {
        pBool = (BOOL *)arrBoolArgs[i];
        ASSERT( pBool != NULL );
        *pBool = *(VARIANT_BOOL *)pBool == VARIANT_FALSE ? FALSE : TRUE;
    }

    if( vtRet != VT_EMPTY ) {
        if( vtRet != VT_VARIANT ) {
            hr = ::VariantChangeType( &varRet, &varRet, 0, vtRet );
            if( FAILED( hr ) ) {
                AfxThrowOleException( hr );
            }
        }

        ASSERT( pvRet != NULL );
        switch( vtRet ) {
        case VT_I2:
            *(SHORT *)pvRet = varRet.iVal;
            break;
        case VT_I4:
            *(LONG *)pvRet = varRet.lVal;
            break;
        case VT_R4:
            *(FLOAT *)pvRet = varRet.fltVal;
            break;
        case VT_R8:
            *(DOUBLE *)pvRet = varRet.dblVal;
            break;
        case VT_CY:
            *(CY *)pvRet = varRet.cyVal;
            break;
        case VT_DATE:
            *(DATE *)pvRet = varRet.date;
            break;
        case VT_BSTR:
            *(CString *)pvRet = varRet.bstrVal;
            ::SysFreeString( varRet.bstrVal );
            break;
        case VT_DISPATCH:
            *(LPDISPATCH *)pvRet = varRet.pdispVal;
            break;
        case VT_ERROR:
            *(SCODE *)pvRet = varRet.scode;
            break;
        case VT_VARIANT:
            memcpy( pvRet, &varRet, sizeof( VARIANTARG ) );
            break;
        case VT_UNKNOWN:
            *(LPUNKNOWN *)pvRet = varRet.punkVal;
            break;
        }
    }
}

void COleDispatchDriver::ReleaseDispatch()
/****************************************/
{
    if( m_bAutoRelease && m_lpDispatch != NULL ) {
        m_lpDispatch->Release();
    }
    m_lpDispatch = NULL;
}

void AFX_CDECL COleDispatchDriver::InvokeHelper( DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
                                                 void *pvRet, const BYTE *pbParamInfo, ... )
/******************************************************************************************/
{
    va_list argList;
    va_start( argList, pbParamInfo );
    InvokeHelperV( dwDispID, wFlags, vtRet, pvRet, pbParamInfo, argList );
    va_end( argList );
}

void AFX_CDECL COleDispatchDriver::SetProperty( DISPID dwDispID, VARTYPE vtProp, ... )
/************************************************************************************/
{
    BYTE bParamInfo[2];
    bParamInfo[0] = (BYTE)vtProp;
    bParamInfo[1] = '\0';
    if( vtProp & VT_BYREF ) {
        bParamInfo[0] |= 0x40;
    }
#ifdef _UNICODE
    if( vtProp == VT_BSTR ) {
        bParamInfo[0] = VT_BSTRA;
    }
#endif
    va_list argList;
    va_start( argList, vtProp );
    if( vtProp == VT_DISPATCH ) {
        InvokeHelperV( dwDispID, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, bParamInfo,
                       argList );
    } else {
        InvokeHelperV( dwDispID, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, bParamInfo,
                       argList );
    }
    va_end( argList );
}
