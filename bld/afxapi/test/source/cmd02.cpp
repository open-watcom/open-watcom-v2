#include "stdafx.h"

interface IInterface1 : IUnknown {
    STDMETHOD( SetValue )( UINT value ) PURE;
};

interface IInterface2 : IUnknown {
    STDMETHOD_( UINT, GetValue )() PURE;
};

// These are fake interface identifiers used only within this test case.  For application
// code, generate an actual valid IID with a tool such a GUIDGEN.
static const IID IID_IInterface1 =
    { 0x00000001, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
static const IID IID_IInterface2 =
    { 0x00000002, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

class CTestInterfaces : public CCmdTarget {
public:
    UINT    m_value;

    CTestInterfaces();

    BEGIN_INTERFACE_PART( Interface1, IInterface1 )
        STDMETHOD( SetValue )( UINT value );
    END_INTERFACE_PART( Interface1 )

    BEGIN_INTERFACE_PART( Interface2, IInterface2 )
        STDMETHOD_( UINT, GetValue )();
    END_INTERFACE_PART( Interface2 )

    DECLARE_INTERFACE_MAP()
};

BEGIN_INTERFACE_MAP( CTestInterfaces, CCmdTarget )
    INTERFACE_PART( CTestInterfaces, IID_IInterface1, Interface1 )
    INTERFACE_PART( CTestInterfaces, IID_IInterface2, Interface2 )
END_INTERFACE_MAP()

CTestInterfaces::CTestInterfaces()
{
    m_value = 0;
}

HRESULT CTestInterfaces::XInterface1::QueryInterface( REFIID riid, void **ppvObject )
{
    METHOD_PROLOGUE( CTestInterfaces, Interface1 )
    return( pThis->InternalQueryInterface( &riid, ppvObject ) );
}

ULONG CTestInterfaces::XInterface1::AddRef()
{
    METHOD_PROLOGUE( CTestInterfaces, Interface1 )
    return( pThis->InternalAddRef() );
}

ULONG CTestInterfaces::XInterface1::Release()
{
    METHOD_PROLOGUE( CTestInterfaces, Interface1 )
    return( pThis->InternalRelease() );
}

HRESULT CTestInterfaces::XInterface1::SetValue( UINT value )
{
    METHOD_PROLOGUE( CTestInterfaces, Interface1 )
    pThis->m_value = value;
    return( S_OK );
}

HRESULT CTestInterfaces::XInterface2::QueryInterface( REFIID riid, void **ppvObject )
{
    METHOD_PROLOGUE( CTestInterfaces, Interface2 )
    return( pThis->InternalQueryInterface( &riid, ppvObject ) );
}

ULONG CTestInterfaces::XInterface2::AddRef()
{
    METHOD_PROLOGUE( CTestInterfaces, Interface2 )
    return( pThis->InternalAddRef() );
}

ULONG CTestInterfaces::XInterface2::Release()
{
    METHOD_PROLOGUE( CTestInterfaces, Interface2 )
    return( pThis->InternalRelease() );
}

UINT CTestInterfaces::XInterface2::GetValue()
{
    METHOD_PROLOGUE( CTestInterfaces, Interface2 )
    return( pThis->m_value );
}

int main()
{
    CTestInterfaces test;

    IInterface1 *pI1 = NULL;
    IInterface2 *pI2 = NULL;

    if( !SUCCEEDED( test.InternalQueryInterface( &IID_IInterface1, (void **)&pI1 ) ) ) _fail;
    if( pI1 != &test.m_xInterface1 ) _fail;
    if( !SUCCEEDED( pI1->SetValue( 1 ) ) ) _fail;
    if( test.m_value != 1 ) _fail;

    if( !SUCCEEDED( pI1->QueryInterface( IID_IInterface2, (void **)&pI2 ) ) ) _fail;
    if( pI2 != &test.m_xInterface2 ) _fail;
    if( pI2->GetValue() != 1 ) _fail;

    if( pI1->Release() != 2 ) _fail;
    if( !SUCCEEDED( pI2->QueryInterface( IID_IInterface1, (void **)&pI1 ) ) ) _fail;
    if( pI1 != &test.m_xInterface1 ) _fail;

    if( pI2->Release() != 2 ) _fail;
    if( pI1->Release() != 1 ) _fail;

    _PASS;
}
