#include "stdafx.h"

class CSerializable : public CObject {
    DECLARE_SERIAL( CSerializable )

public:
    int m_n1;
    int m_n2;

    virtual void    Serialize( CArchive &ar );
};

IMPLEMENT_SERIAL( CSerializable, CObject, 1 )

void CSerializable::Serialize( CArchive &ar )
{
    if( ar.IsStoring() ) {
        ar << m_n1;
        ar << m_n2;
    } else {
        ar >> m_n1;
        ar >> m_n2;
    }
}

int main()
{
    CSerializable s1;
    CSerializable s2;

    s1.m_n1 = 1;
    s1.m_n2 = 2;
    s2.m_n1 = 3;
    s2.m_n2 = 4;

    CMemFile file;
    CArchive ar( &file, CArchive::store );
    ar.MapObject( &s1 );
    ar << (CObject *)&s1;
    ar << (CObject *)&s2;
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    ar2.MapObject( &s1 );

    CObject *pOb1 = NULL;
    CObject *pOb2 = NULL;
    ar2 >> pOb1;
    ar2 >> pOb2;
    if( pOb1 == NULL ) _fail;
    if( pOb2 == NULL ) _fail;
    if( pOb1 != &s1 ) _fail;
    if( pOb2 == &s1 ) _fail;
    if( pOb2 == &s2 ) _fail;
    if( !pOb2->IsKindOf( RUNTIME_CLASS( CSerializable ) ) ) _fail;
    if( ((CSerializable *)pOb1)->m_n1 != 1 ) _fail;
    if( ((CSerializable *)pOb1)->m_n2 != 2 ) _fail;
    if( ((CSerializable *)pOb2)->m_n1 != 3 ) _fail;
    if( ((CSerializable *)pOb2)->m_n2 != 4 ) _fail;
    ar2.Close();

    // Don't delete pOb1 because it should contain apointer to s1, which is on the stack.
    delete pOb2;
    
    _PASS;
}
