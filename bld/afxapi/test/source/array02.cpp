#include "stdafx.h"

int main()
{
    for( int i = 1; i <= 5; i++ ) {
        CWordArray arr1;
        CWordArray arr2;
        CWordArray arr3;

        arr1.SetSize( 0, i );
        arr2.SetSize( 0, i );
        arr3.SetSize( 0, i );

        if( !arr1.IsEmpty() ) _fail;

        arr1.Add( 0 );
        arr1.Add( 1 );
        arr1.Add( 2 );
        arr2.Add( 3 );
        arr2.Add( 4 );
        arr2.Add( 5 );
        arr1.Append( arr2 );
        arr3.Copy( arr1 );
        arr3.SetAt( 0, 1 );
        arr3.RemoveAt( 1 );

        if( arr3.GetAt( 0 ) != 1 ) _fail;
        if( arr3.GetAt( 1 ) != 2 ) _fail;
        if( arr3.GetAt( 2 ) != 3 ) _fail;
        if( arr3.GetAt( 3 ) != 4 ) _fail;
        if( arr3.GetAt( 4 ) != 5 ) _fail;
        if( arr3.GetSize() != 5 ) _fail;
        if( arr3.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, 1, 2 );
        if( arr1.GetAt( 0 ) != 3 ) _fail;
        if( arr1.GetAt( 1 ) != 1 ) _fail;
        if( arr1.GetAt( 2 ) != 1 ) _fail;
        if( arr1.GetAt( 3 ) != 4 ) _fail;
        if( arr1.GetAt( 4 ) != 5 ) _fail;
        if( arr1.GetSize() != 5 ) _fail;
        if( arr1.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, &arr2 );
        arr1.SetAtGrow( 7, 6 );
        if( arr1.GetAt( 0 ) != 3 ) _fail;
        if( arr1.GetAt( 1 ) != 3 ) _fail;
        if( arr1.GetAt( 2 ) != 4 ) _fail;
        if( arr1.GetAt( 3 ) != 5 ) _fail;
        if( arr1.GetAt( 4 ) != 4 ) _fail;
        if( arr1.GetAt( 5 ) != 5 ) _fail;
        if( arr1.GetAt( 6 ) != 0 ) _fail;
        if( arr1.GetAt( 7 ) != 6 ) _fail;
        if( arr1.GetSize() != 8 ) _fail;
        if( arr1.GetUpperBound() != 7 ) _fail;

        arr1.SetSize( 10 );
        if( arr1.GetSize() != 10 ) _fail;
        if( arr1.GetUpperBound() != 9 ) _fail;
        if( arr1.GetAt( 8 ) != 0 ) _fail;
        if( arr1.GetAt( 9 ) != 0 ) _fail;
    }

    CWordArray sarr1;
    CWordArray sarr2;
    sarr1.SetSize( 10 );
    for( int i = 0; i < 10; i++ ) {
        sarr1.SetAt( i, i );
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
