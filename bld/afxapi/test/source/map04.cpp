#include "stdafx.h"

int main()
{
    CMapWordToOb map;

    if( !map.IsEmpty() ) _fail;
    if( map.GetCount() != 0 ) _fail;
    if( map.GetSize() != 0 ) _fail;

    map.SetAt( 0, (CObject *)0 );
    map.SetAt( 1, (CObject *)1 );
    map.SetAt( 2, (CObject *)2 );

    CObject *value;
    if( !map.Lookup( 0, value ) ) _fail;
    if( value != (CObject *)0 ) _fail;
    if( !map.Lookup( 1, value ) ) _fail;
    if( value != (CObject *)1 ) _fail;
    if( !map.Lookup( 2, value ) ) _fail;
    if( value != (CObject *)2 ) _fail;
    if( map.Lookup( 3, value ) ) _fail;

    POSITION    position = map.GetStartPosition();
    WORD        key;
    BOOL        bFound0 = FALSE;
    BOOL        bFound1 = FALSE;
    BOOL        bFound2 = FALSE;
    while( position != NULL ) {
        map.GetNextAssoc( position, key, value );
        switch( key ) {
        case 0:
            if( bFound0 ) _fail;
            if( value != (CObject *)0 ) _fail;
            bFound0 = TRUE;
            break;
        case 1:
            if( bFound1 ) _fail;
            if( value != (CObject *)1 ) _fail;
            bFound1 = TRUE;
            break;
        case 2:
            if( bFound2 ) _fail;
            if( value != (CObject *)2 ) _fail;
            bFound2 = TRUE;
            break;
        default:
            _fail;
            break;
        }
    }
    if( !bFound0 ) _fail;
    if( !bFound1 ) _fail;
    if( !bFound2 ) _fail;

    map.RemoveKey( 0 );
    if( map.Lookup( 0, value ) ) _fail;

    CMapWordToOb smap1;
    CMapWordToOb smap2;
    smap1[0] = (CObject *)0;
    smap1[1] = (CObject *)1;
    smap1[2] = (CObject *)2;
    
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    ar.MapObject( (CObject *)1 );
    ar.MapObject( (CObject *)2 );
    smap1.Serialize( ar );
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    ar2.MapObject( (CObject *)1 );
    ar2.MapObject( (CObject *)2 );
    smap2.Serialize( ar2 );
    ar2.Close();

    if( smap2[0] != (CObject *)0 ) _fail;
    if( smap2[1] != (CObject *)1 ) _fail;
    if( smap2[2] != (CObject *)2 ) _fail;

    _PASS;
}
