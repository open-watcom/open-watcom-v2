#include "stdafx.h"

class CCreateable : public CObject {
public:
    int m_nTest;

    DECLARE_DYNCREATE( CCreateable )

protected:
    CCreateable();
};

IMPLEMENT_DYNCREATE( CCreateable, CObject )

CCreateable::CCreateable()
{
    m_nTest = 1;
}

int main()
{
    CRuntimeClass *pClass = RUNTIME_CLASS( CCreateable );
    if( pClass == NULL ) _fail;

    CCreateable *pOb = (CCreateable *)pClass->CreateObject();
    if( pOb == NULL ) _fail;
    if( !pOb->IsKindOf( RUNTIME_CLASS( CObject ) ) ) _fail;
    if( !pOb->IsKindOf( RUNTIME_CLASS( CCreateable ) ) ) _fail;
    if( pOb->m_nTest != 1 ) _fail;
    delete pOb;

    _PASS;
}
