#include "stdafx.h"

int main()
{
    for( int i = 1; i <= 5; i++ ) {
        CPtrArray arr1;
        CPtrArray arr2;
        CPtrArray arr3;

        arr1.SetSize( 0, i );
        arr2.SetSize( 0, i );
        arr3.SetSize( 0, i );

        if( !arr1.IsEmpty() ) _fail;

        arr1.Add( (void *)0 );
        arr1.Add( (void *)1 );
        arr1.Add( (void *)2 );
        arr2.Add( (void *)3 );
        arr2.Add( (void *)4 );
        arr2.Add( (void *)5 );
        arr1.Append( arr2 );
        arr3.Copy( arr1 );
        arr3.SetAt( 0, (void *)1 );
        arr3.RemoveAt( 1 );

        if( arr3.GetAt( 0 ) != (void *)1 ) _fail;
        if( arr3.GetAt( 1 ) != (void *)2 ) _fail;
        if( arr3.GetAt( 2 ) != (void *)3 ) _fail;
        if( arr3.GetAt( 3 ) != (void *)4 ) _fail;
        if( arr3.GetAt( 4 ) != (void *)5 ) _fail;
        if( arr3.GetSize() != 5 ) _fail;
        if( arr3.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, (void *)1, 2 );
        if( arr1.GetAt( 0 ) != (void *)3 ) _fail;
        if( arr1.GetAt( 1 ) != (void *)1 ) _fail;
        if( arr1.GetAt( 2 ) != (void *)1 ) _fail;
        if( arr1.GetAt( 3 ) != (void *)4 ) _fail;
        if( arr1.GetAt( 4 ) != (void *)5 ) _fail;
        if( arr1.GetSize() != 5 ) _fail;
        if( arr1.GetUpperBound() != 4 ) _fail;

        arr1.Copy( arr2 );
        arr1.InsertAt( 1, &arr2 );
        arr1.SetAtGrow( 7, (void *)6 );
        if( arr1.GetAt( 0 ) != (void *)3 ) _fail;
        if( arr1.GetAt( 1 ) != (void *)3 ) _fail;
        if( arr1.GetAt( 2 ) != (void *)4 ) _fail;
        if( arr1.GetAt( 3 ) != (void *)5 ) _fail;
        if( arr1.GetAt( 4 ) != (void *)4 ) _fail;
        if( arr1.GetAt( 5 ) != (void *)5 ) _fail;
        if( arr1.GetAt( 6 ) != NULL ) _fail;
        if( arr1.GetAt( 7 ) != (void *)6 ) _fail;
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
