#include "stdafx.h"

// Dispatch identifiers for test class
#define DISPID_SIMPLEFLAGTEST   1
#define DISPID_RETURNTEST       2
#define DISPID_PARAMTEST1       3
#define DISPID_PARAMTEST2       4
#define DISPID_PARAMTEST3       5
#define DISPID_REFTEST1         6
#define DISPID_REFTEST2         7
#define DISPID_PROPTEST1        8

class CTestDispatch : public IDispatch {
public:
    BOOL    m_bFlag;
    LONG    m_lPropTest1;

    CTestDispatch();

    STDMETHOD( QueryInterface )( REFIID riid, void **ppvObject );
    STDMETHOD_( ULONG, AddRef )();
    STDMETHOD_( ULONG, Release )();
    STDMETHOD( GetTypeInfoCount )( unsigned int *pctinfo );
    STDMETHOD( GetTypeInfo )( unsigned int iTInfo, LCID lcid, ITypeInfo **ppTInfo );
    STDMETHOD( GetIDsOfNames )( REFIID riid, OLECHAR **rgszNames, unsigned int cNames, LCID lcid, DISPID *rgDispId );
    STDMETHOD( Invoke )( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, unsigned int *puArgErr );
};

CTestDispatch::CTestDispatch()
{
    m_bFlag = FALSE;
    m_lPropTest1 = 0L;
}

HRESULT CTestDispatch::QueryInterface( REFIID riid, void **ppvObject )
{
    if( riid == IID_IUnknown || riid == IID_IDispatch ) {
        *ppvObject = this;
        return( S_OK );
    }
    return( E_NOINTERFACE );
}

ULONG CTestDispatch::AddRef()
{
    return( 1L );
}

ULONG CTestDispatch::Release()
{
    return( 0L );
}

HRESULT CTestDispatch::GetTypeInfoCount( unsigned int *pctinfo )
{
    UNUSED_ALWAYS( pctinfo );
    return( E_NOTIMPL );
}

HRESULT CTestDispatch::GetTypeInfo( unsigned int iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
    UNUSED_ALWAYS( iTInfo );
    UNUSED_ALWAYS( lcid );
    UNUSED_ALWAYS( ppTInfo );
    return( E_NOTIMPL );
}

HRESULT CTestDispatch::GetIDsOfNames( REFIID riid, OLECHAR **rgszNames, unsigned int cNames,
                                      LCID lcid, DISPID *rgDispId )
{
    UNUSED_ALWAYS( riid );
    UNUSED_ALWAYS( rgszNames );
    UNUSED_ALWAYS( cNames );
    UNUSED_ALWAYS( lcid );
    UNUSED_ALWAYS( rgDispId );
    return( E_NOTIMPL );
}

HRESULT CTestDispatch::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                               DISPPARAMS *pDispParams, VARIANT *pVarResult,
                               EXCEPINFO *pExcepInfo, unsigned int *puArgErr )
{
    UNUSED_ALWAYS( riid );
    UNUSED_ALWAYS( lcid );
    UNUSED_ALWAYS( pVarResult );
    UNUSED_ALWAYS( pExcepInfo );
    UNUSED_ALWAYS( puArgErr );
    if( wFlags == DISPATCH_METHOD ) {
        switch( dispIdMember ) {
        case DISPID_SIMPLEFLAGTEST:
            if( pDispParams->cArgs != 0 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            m_bFlag = TRUE;
            return( S_OK );
        case DISPID_RETURNTEST:
            if( pDispParams->cArgs != 0 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            pVarResult->vt = VT_I4;
            pVarResult->lVal = 5L;
            return( S_OK );
        case DISPID_PARAMTEST1:
            if( pDispParams->cArgs != 1 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[0].vt != VT_I4 ) {
                return( DISP_E_TYPEMISMATCH );
            }
            if( pDispParams->rgvarg[0].lVal != 5L ) _fail;
            return( S_OK );
        case DISPID_PARAMTEST2:
            if( pDispParams->cArgs != 2 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[1].vt != VT_I4 || pDispParams->rgvarg[0].vt != VT_I4 ) {
                return( DISP_E_TYPEMISMATCH );
            }
            if( pDispParams->rgvarg[1].lVal != 1L ) _fail;
            if( pDispParams->rgvarg[0].lVal != 2L ) _fail;
            return( S_OK );
        case DISPID_PARAMTEST3:
            if( pDispParams->cArgs != 1 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[0].vt != VT_BSTR ) {
                return( DISP_E_TYPEMISMATCH );
            }
            if( wcscmp( pDispParams->rgvarg[0].bstrVal, L"Test" ) != 0 ) _fail;
            return( S_OK );
        case DISPID_REFTEST1:
            if( pDispParams->cArgs != 1 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[0].vt != (VT_I4 | VT_BYREF) ) {
                return( DISP_E_TYPEMISMATCH );
            }
            *(LONG *)pDispParams->rgvarg[0].plVal = 5L;
            return( S_OK );
        case DISPID_REFTEST2:
            if( pDispParams->cArgs != 1 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[0].vt != (VT_BOOL | VT_BYREF) ) {
                return( DISP_E_TYPEMISMATCH );
            }
            if( *pDispParams->rgvarg[0].pboolVal != VARIANT_FALSE ) _fail;
            *pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
            return( S_OK );
        }
    } else if( wFlags == DISPATCH_PROPERTYGET ) {
        switch( dispIdMember ) {
        case DISPID_PROPTEST1:
            if( pDispParams->cArgs != 0 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            pVarResult->vt = VT_I4;
            pVarResult->lVal = m_lPropTest1;
            return( S_OK );
        }
    } else if( wFlags == DISPATCH_PROPERTYPUT ) {
        switch( dispIdMember ) {
        case DISPID_PROPTEST1:
            if( pDispParams->cArgs != 1 ) {
                return( DISP_E_BADPARAMCOUNT );
            }
            if( pDispParams->rgvarg[0].vt != VT_I4 ) {
                return( DISP_E_TYPEMISMATCH );
            }
            m_lPropTest1 = pDispParams->rgvarg[0].lVal;
            return( S_OK );
        }
    }
    return( DISP_E_MEMBERNOTFOUND );
}

int main()
{
    CTestDispatch disp;
    COleDispatchDriver drv( &disp, FALSE );

    drv.InvokeHelper( DISPID_SIMPLEFLAGTEST, DISPATCH_METHOD, VT_EMPTY, NULL, (BYTE *)"" );
    if( !disp.m_bFlag ) _fail;

    LONG lRet = 0L;
    drv.InvokeHelper( DISPID_RETURNTEST, DISPATCH_METHOD, VT_I4, &lRet, (BYTE *)"" );
    if( lRet != 5L ) _fail;

    drv.InvokeHelper( DISPID_PARAMTEST1, DISPATCH_METHOD, VT_EMPTY, NULL,
                      (BYTE *)VTS_I4, 5L );
    drv.InvokeHelper( DISPID_PARAMTEST2, DISPATCH_METHOD, VT_EMPTY, NULL,
                      (BYTE *)VTS_I4 VTS_I4, 1L, 2L );
    drv.InvokeHelper( DISPID_PARAMTEST3, DISPATCH_METHOD, VT_EMPTY, NULL,
                      (BYTE *)VTS_BSTR, _T("Test") );

    LONG lVal = 0L;
    drv.InvokeHelper( DISPID_REFTEST1, DISPATCH_METHOD, VT_EMPTY, NULL,
                      (BYTE *)VTS_PI4, &lVal );
    if( lVal != 5L ) _fail;

    BOOL bVal = FALSE;
    drv.InvokeHelper( DISPID_REFTEST2, DISPATCH_METHOD, VT_EMPTY, NULL,
                      (BYTE *)VTS_PBOOL, &bVal );
    if( bVal != TRUE ) _fail;

    drv.InvokeHelper( DISPID_PROPTEST1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL,
                      (BYTE *)VTS_I4, 5L );
    if( disp.m_lPropTest1 != 5L ) _fail;
    drv.InvokeHelper( DISPID_PROPTEST1, DISPATCH_PROPERTYGET, VT_I4, &lVal, (BYTE *)"" );
    if( lVal != 5L ) _fail;
    drv.SetProperty( DISPID_PROPTEST1, VT_I4, 10L );
    if( disp.m_lPropTest1 != 10L ) _fail;
    drv.GetProperty( DISPID_PROPTEST1, VT_I4, &lVal );
    if( lVal != 10L ) _fail;

    _PASS;
}
