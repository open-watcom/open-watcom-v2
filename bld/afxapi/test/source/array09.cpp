#include "stdafx.h"

typedef CTypedPtrArray< CPtrArray, int * >  CIntPtrArray;

int main()
{
    for( int i = 1; i <= 5; i++ ) {
        CIntPtrArray arr1;
        CIntPtrArray arr2;
        CIntPtrArray arr3;

        arr1.SetSize( 0, i );
        arr2.SetSize( 0, i );
        arr3.SetSize( 0, i );

        if( !arr1.IsEmpty() ) _fail;

        arr1.Add( (int *)0 );
        arr1.Add( (int *)1 );
        arr1.Add( (int *)2 );
        arr2.Add( (int *)3 );
        arr2.Add( (int *)4 );
        arr2.Add( (int *)5 );
        arr1.Append( arr2 );
        arr3.Copy( arr1 );
        arr3.SetAt( 0, (int *)1 );
        arr3.RemoveAt( 1 );

        if( arr3.GetAt( 0 ) != (int *)1 ) _fail;
        if( arr3.GetAt( 1 ) != (int *)2 ) _fail;
        if( arr3.GetAt( 2 ) != (int *)3 ) _fail;
        if( arr3.GetAt( 3 ) != (int *)4 ) _fail;
        if( arr3.GetAt( 4 ) != (int *)5 ) _fail;
        if( arr3.GetSize() != 5 ) _fail;
        if( arr3.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, (int *)1, 2 );
        if( arr1.GetAt( 0 ) != (int *)3 ) _fail;
        if( arr1.GetAt( 1 ) != (int *)1 ) _fail;
        if( arr1.GetAt( 2 ) != (int *)1 ) _fail;
        if( arr1.GetAt( 3 ) != (int *)4 ) _fail;
        if( arr1.GetAt( 4 ) != (int *)5 ) _fail;
        if( arr1.GetSize() != 5 ) _fail;
        if( arr1.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, &arr2 );
        arr1.SetAtGrow( 7, (int *)6 );
        if( arr1.GetAt( 0 ) != (int *)3 ) _fail;
        if( arr1.GetAt( 1 ) != (int *)3 ) _fail;
        if( arr1.GetAt( 2 ) != (int *)4 ) _fail;
        if( arr1.GetAt( 3 ) != (int *)5 ) _fail;
        if( arr1.GetAt( 4 ) != (int *)4 ) _fail;
        if( arr1.GetAt( 5 ) != (int *)5 ) _fail;
        if( arr1.GetAt( 6 ) != NULL ) _fail;
        if( arr1.GetAt( 7 ) != (int *)6 ) _fail;
        if( arr1.GetSize() != 8 ) _fail;
        if( arr1.GetUpperBound() != 7 ) _fail;

        arr1.SetSize( 10 );
        if( arr1.GetSize() != 10 ) _fail;
        if( arr1.GetUpperBound() != 9 ) _fail;
        if( arr1.GetAt( 8 ) != NULL ) _fail;
        if( arr1.GetAt( 9 ) != NULL ) _fail;
    }

    _PASS;
}
