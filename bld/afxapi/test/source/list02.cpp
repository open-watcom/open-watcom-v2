#include "stdafx.h"

int main()
{
    CObList list1;
    CObList list2;

    if( list1.GetHeadPosition() != NULL ) _fail;
    if( list1.GetTailPosition() != NULL ) _fail;

    list1.AddTail( (CObject *)1 );
    list1.AddTail( (CObject *)2 );
    list1.AddHead( (CObject *)0 );
    if( list1.GetCount() != 3 ) _fail;
    if( list1.GetSize() != 3 ) _fail;
    if( list1.GetHead() != (CObject *)0 ) _fail;
    if( list1.GetTail() != (CObject *)2 ) _fail;

    POSITION position = list1.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (CObject *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (CObject *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetNext( position ) != (CObject *)2 ) _fail;
    if( position != NULL ) _fail;

    position = list1.GetTailPosition();
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (CObject *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (CObject *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetPrev( position ) != (CObject *)0 ) _fail;
    if( position != NULL ) _fail;

    position = list1.Find( (CObject *)0 );
    if( position != list1.GetHeadPosition() ) _fail;
    position = list1.Find( (CObject *)1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (CObject *)2 );
    if( position != list1.GetTailPosition() ) _fail;
    position = list1.Find( (CObject *)3 );
    if( position != NULL ) _fail;

    POSITION posStart = list1.Find( (CObject *)1 );
    position = list1.Find( (CObject *)0, posStart );
    if( position != NULL ) _fail;
    position = list1.Find( (CObject *)1, posStart );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    position = list1.Find( (CObject *)2, posStart );
    if( position != list1.GetTailPosition() ) _fail;

    position = list1.FindIndex( 0 );
    if( position != list1.GetHeadPosition() ) _fail;
    if( list1.GetAt( position ) != (CObject *)0 ) _fail;
    position = list1.FindIndex( 1 );
    if( position == list1.GetHeadPosition() ) _fail;
    if( position == list1.GetTailPosition() ) _fail;
    if( position == NULL ) _fail;
    if( list1.GetAt( position ) != (CObject *)1 ) _fail;
    position = list1.FindIndex( 2 );
    if( position != list1.GetTailPosition() ) _fail;
    if( list1.GetAt( position ) != (CObject *)2 ) _fail;
    position = list1.FindIndex( 3 );
    if( position != NULL ) _fail;

    list2.AddTail( (CObject *)3 );
    list2.AddHead( &list1 );
    list2.AddTail( &list1 );
    if( list2.GetCount() != 7 ) _fail;
    if( list2.GetSize() != 7 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)0 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)2 ) _fail;
    if( position != NULL ) _fail;

    if( list2.RemoveHead() != (CObject *)0 ) _fail;
    if( list2.RemoveTail() != (CObject *)2 ) _fail;
    position = list2.Find( (CObject *)0 );
    if( position == NULL ) _fail;
    list2.RemoveAt( position );
    if( list2.GetCount() != 4 ) _fail;
    if( list2.GetSize() != 4 ) _fail;
    position = list2.GetHeadPosition();
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)1 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)2 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)3 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetNext( position ) != (CObject *)1 ) _fail;
    if( position != NULL ) _fail;

    position = list2.GetTailPosition();
    list2.SetAt( position, (CObject *)5 );
    list2.InsertBefore( position, (CObject *)4 );
    list2.InsertAfter( position, (CObject *)6 );
    position = list2.GetTailPosition();
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (CObject *)6 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (CObject *)5 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (CObject *)4 ) _fail;
    if( position == NULL ) _fail;
    if( list2.GetPrev( position ) != (CObject *)3 ) _fail;

    CObList slist1;
    CObList slist2;
    for( int i = 0; i < 10; i++ ) {
        slist1.AddTail( (CObject *)i );
    }
    
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    for( int i = 1; i < 10; i++ ) {
        ar.MapObject( (CObject *)i );
    }
    slist1.Serialize( ar );
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    for( int i = 1; i < 10; i++ ) {
        ar2.MapObject( (CObject *)i );
    }
    slist2.Serialize( ar2 );
    ar2.Close();

    if( slist1.GetSize() != slist2.GetSize() ) _fail;

    POSITION position1 = slist1.GetHeadPosition();
    POSITION position2 = slist2.GetHeadPosition();
    while( position1 != NULL && position2 != NULL ) {
        if( slist1.GetNext( position1 ) != slist2.GetNext( position2 ) ) _fail;
    }
    if( position1 != NULL ) _fail;
    if( position2 != NULL ) _fail;

    _PASS;
}
