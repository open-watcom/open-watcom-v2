#include "stdafx.h"

int main()
{
    for( int i = 1; i <= 5; i++ ) {
        CStringArray arr1;
        CStringArray arr2;
        CStringArray arr3;

        arr1.SetSize( 0, i );
        arr2.SetSize( 0, i );
        arr3.SetSize( 0, i );

        if( !arr1.IsEmpty() ) _fail;

        arr1.Add( _T("0") );
        arr1.Add( _T("1") );
        arr1.Add( _T("2") );
        arr2.Add( _T("3") );
        arr2.Add( _T("4") );
        arr2.Add( _T("5") );
        arr1.Append( arr2 );
        arr3.Copy( arr1 );
        arr3.SetAt( 0, _T("1") );
        arr3.RemoveAt( 1 );

        if( arr3.GetAt( 0 ) != _T("1") ) _fail;
        if( arr3.GetAt( 1 ) != _T("2") ) _fail;
        if( arr3.GetAt( 2 ) != _T("3") ) _fail;
        if( arr3.GetAt( 3 ) != _T("4") ) _fail;
        if( arr3.GetAt( 4 ) != _T("5") ) _fail;
        if( arr3.GetSize() != 5 ) _fail;
        if( arr3.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, _T("1"), 2 );
        if( arr1.GetAt( 0 ) != _T("3") ) _fail;
        if( arr1.GetAt( 1 ) != _T("1") ) _fail;
        if( arr1.GetAt( 2 ) != _T("1") ) _fail;
        if( arr1.GetAt( 3 ) != _T("4") ) _fail;
        if( arr1.GetAt( 4 ) != _T("5") ) _fail;
        if( arr1.GetSize() != 5 ) _fail;
        if( arr1.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, &arr2 );
        arr1.SetAtGrow( 7, _T("6") );
        if( arr1.GetAt( 0 ) != _T("3") ) _fail;
        if( arr1.GetAt( 1 ) != _T("3") ) _fail;
        if( arr1.GetAt( 2 ) != _T("4") ) _fail;
        if( arr1.GetAt( 3 ) != _T("5") ) _fail;
        if( arr1.GetAt( 4 ) != _T("4") ) _fail;
        if( arr1.GetAt( 5 ) != _T("5") ) _fail;
        if( !arr1.GetAt( 6 ).IsEmpty() ) _fail;
        if( arr1.GetAt( 7 ) != _T("6") ) _fail;
        if( arr1.GetSize() != 8 ) _fail;
        if( arr1.GetUpperBound() != 7 ) _fail;

        arr1.SetSize( 10 );
        if( arr1.GetSize() != 10 ) _fail;
        if( arr1.GetUpperBound() != 9 ) _fail;
        if( !arr1.GetAt( 8 ).IsEmpty() ) _fail;
        if( !arr1.GetAt( 9 ).IsEmpty() ) _fail;
    }

    CStringArray sarr1;
    CStringArray sarr2;
    sarr1.SetSize( 10 );
    for( int i = 0; i < 10; i++ ) {
        CString str;
        str.Format( _T("%d"), i );
        sarr1.SetAt( i, str );
    }
    
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    sarr1.Serialize( ar );
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    sarr2.Serialize( ar2 );
    ar2.Close();

    if( sarr1.GetSize() != sarr2.GetSize() ) _fail;
    for( int i = 0; i < sarr1.GetSize(); i++ ) {
        if( sarr1.GetAt( i ) != sarr2.GetAt( i ) ) _fail;
    }

    _PASS;
}
