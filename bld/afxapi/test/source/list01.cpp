#include "stdafx.h"

int main()
{
    CPtrList list1;
    CPtrList list2;

    if( list1.GetHeadPosition() != NULL ) _fail;
    if( list1.GetTailPosition() != NULL ) _fail;

    list1.AddTail( (void *)1 );
    list1.AddTail( (void *)2 );
    list1.AddHead( (void *)0 );
    if( list1.GetCount() != 3 ) _fail;
    if( list1.GetSize() != 3 ) _fail;
    if( list1.GetHead() != (void *)0 ) _fail;
    if( list1.GetTail() != (void *)2 ) _fail;

    POSITION position = list1.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (void *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (void *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (void *)2 ) _fail;
    if( position != NULL ) _fail;

    position = list1.GetTailPosition();
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (void *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (void *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (void *)0 ) _fail;
    if( position != NULL ) _fail;

    position = list1.Find( (void *)0 );
    if( position != list1.GetHeadPosition() ) _fail;
    position = list1.Find( (void *)1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (void *)2 );
    if( position != list1.GetTailPosition() ) _fail;
    position = list1.Find( (void *)3 );
    if( position != NULL ) _fail;

    POSITION posStart = list1.Find( (void *)1 );
    position = list1.Find( (void *)0, posStart );
    if( position != NULL ) _fail;
    position = list1.Find( (void *)1, posStart );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (void *)2, posStart );
    if( position != list1.GetTailPosition() ) _fail;

    position = list1.FindIndex( 0 );
    if( position != list1.GetHeadPosition() ) _fail;
    if( list1.GetAt( position ) != (void *)0 ) _fail;
    position = list1.FindIndex( 1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetAt( position ) != (void *)1 ) _fail;
    position = list1.FindIndex( 2 );
    if( position != list1.GetTailPosition() ) _fail;
    if( list1.GetAt( position ) != (void *)2 ) _fail;
    position = list1.FindIndex( 3 );
    if( position != NULL ) _fail;

    list2.AddTail( (void *)3 );
    list2.AddHead( &list1 );
    list2.AddTail( &list1 );
    if( list2.GetCount() != 7 ) _fail;
    if( list2.GetSize() != 7 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)2 ) _fail;
    if( position != NULL ) _fail;

    if( list2.RemoveHead() != (void *)0 ) _fail;
    if( list2.RemoveTail() != (void *)2 ) _fail;
    position = list2.Find( (void *)0 );
    if( position == NULL ) _fail;
    list2.RemoveAt( position );
    if( list2.GetCount() != 4 ) _fail;
    if( list2.GetSize() != 4 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (void *)1 ) _fail;
    if( position != NULL ) _fail;

    position = list2.GetTailPosition();
    list2.SetAt( position, (void *)5 );
    list2.InsertBefore( position, (void *)4 );
    list2.InsertAfter( position, (void *)6 );
    position = list2.GetTailPosition();
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (void *)6 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (void *)5 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (void *)4 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (void *)3 ) _fail;

    _PASS;
}
