#include "stdafx.h"

typedef CTypedPtrList< CPtrList, int * >    CIntPtrList;

int main()
{
    CIntPtrList list1;
    CIntPtrList list2;

    if( list1.GetHeadPosition() != NULL ) _fail;
    if( list1.GetTailPosition() != NULL ) _fail;

    list1.AddTail( (int *)1 );
    list1.AddTail( (int *)2 );
    list1.AddHead( (int *)0 );
    if( list1.GetCount() != 3 ) _fail;
    if( list1.GetSize() != 3 ) _fail;
    if( list1.GetHead() != (int *)0 ) _fail;
    if( list1.GetTail() != (int *)2 ) _fail;

    POSITION position = list1.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (int *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (int *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (int *)2 ) _fail;
    if( position != NULL ) _fail;

    position = list1.GetTailPosition();
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (int *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (int *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (int *)0 ) _fail;
    if( position != NULL ) _fail;

    position = list1.Find( (int *)0 );
    if( position != list1.GetHeadPosition() ) _fail;
    position = list1.Find( (int *)1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (int *)2 );
    if( position != list1.GetTailPosition() ) _fail;
    position = list1.Find( (int *)3 );
    if( position != NULL ) _fail;

    POSITION posStart = list1.Find( (int *)1 );
    position = list1.Find( (int *)0, posStart );
    if( position != NULL ) _fail;
    position = list1.Find( (int *)1, posStart );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (int *)2, posStart );
    if( position != list1.GetTailPosition() ) _fail;

    position = list1.FindIndex( 0 );
    if( position != list1.GetHeadPosition() ) _fail;
    if( list1.GetAt( position ) != (int *)0 ) _fail;
    position = list1.FindIndex( 1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetAt( position ) != (int *)1 ) _fail;
    position = list1.FindIndex( 2 );
    if( position != list1.GetTailPosition() ) _fail;
    if( list1.GetAt( position ) != (int *)2 ) _fail;
    position = list1.FindIndex( 3 );
    if( position != NULL ) _fail;

    list2.AddTail( (int *)3 );
    list2.AddHead( &list1 );
    list2.AddTail( &list1 );
    if( list2.GetCount() != 7 ) _fail;
    if( list2.GetSize() != 7 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)2 ) _fail;
    if( position != NULL ) _fail;

    if( list2.RemoveHead() != (int *)0 ) _fail;
    if( list2.RemoveTail() != (int *)2 ) _fail;
    position = list2.Find( (int *)0 );
    if( position == NULL ) _fail;
    list2.RemoveAt( position );
    if( list2.GetCount() != 4 ) _fail;
    if( list2.GetSize() != 4 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (int *)1 ) _fail;
    if( position != NULL ) _fail;

    position = list2.GetTailPosition();
    list2.SetAt( position, (int *)5 );
    list2.InsertBefore( position, (int *)4 );
    list2.InsertAfter( position, (int *)6 );
    position = list2.GetTailPosition();
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (int *)6 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (int *)5 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (int *)4 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (int *)3 ) _fail;

    _PASS;
}
