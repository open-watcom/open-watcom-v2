#include "stdafx.h"

class CBaseClass1 : public CObject {
    DECLARE_DYNAMIC( CBaseClass1 )
};

class CBaseClass2 : public CObject {
    DECLARE_DYNAMIC( CBaseClass2 )
};

class CDerivedClass : public CBaseClass1 {
    DECLARE_DYNAMIC( CDerivedClass )
};

IMPLEMENT_DYNAMIC( CBaseClass1, CObject )
IMPLEMENT_DYNAMIC( CBaseClass2, CObject )
IMPLEMENT_DYNAMIC( CDerivedClass, CBaseClass1 )

int main()
{
    CBaseClass1 b1;
    CBaseClass2 b2;
    CDerivedClass d;

    if( !b1.IsKindOf( RUNTIME_CLASS( CObject ) ) ) _fail;
    if( !b2.IsKindOf( RUNTIME_CLASS( CObject ) ) ) _fail;
    if( !d.IsKindOf( RUNTIME_CLASS( CObject ) ) ) _fail;
    if( !b1.IsKindOf( RUNTIME_CLASS( CBaseClass1 ) ) ) _fail;
    if( b2.IsKindOf( RUNTIME_CLASS( CBaseClass1 ) ) ) _fail;
    if( !d.IsKindOf( RUNTIME_CLASS( CBaseClass1 ) ) ) _fail;
    if( b1.IsKindOf( RUNTIME_CLASS( CBaseClass2 ) ) ) _fail;
    if( !b2.IsKindOf( RUNTIME_CLASS( CBaseClass2 ) ) ) _fail;
    if( d.IsKindOf( RUNTIME_CLASS( CBaseClass2 ) ) ) _fail;
    if( b1.IsKindOf( RUNTIME_CLASS( CDerivedClass ) ) ) _fail;
    if( b2.IsKindOf( RUNTIME_CLASS( CDerivedClass ) ) ) _fail;
    if( !d.IsKindOf( RUNTIME_CLASS( CDerivedClass ) ) ) _fail;
    _PASS;
}
