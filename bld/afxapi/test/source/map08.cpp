#include "stdafx.h"

typedef CMap< int, int, int, int >  CMapIntToInt;

int main()
{
    CMapIntToInt map;

    if( !map.IsEmpty() ) _fail;
    if( map.GetCount() != 0 ) _fail;
    if( map.GetSize() != 0 ) _fail;

    map.SetAt( 0, 0 );
    map.SetAt( 1, 1 );
    map.SetAt( 2, 2 );

    int value;
    if( !map.Lookup( 0, value ) ) _fail;
    if( value != 0 ) _fail;
    if( !map.Lookup( 1, value ) ) _fail;
    if( value != 1 ) _fail;
    if( !map.Lookup( 2, value ) ) _fail;
    if( value != 2 ) _fail;
    if( map.Lookup( 3, value ) ) _fail;

    POSITION    position = map.GetStartPosition();
    int         key;
    BOOL        bFound0 = FALSE;
    BOOL        bFound1 = FALSE;
    BOOL        bFound2 = FALSE;
    while( position != NULL ) {
        map.GetNextAssoc( position, key, value );
        switch( key ) {
        case 0:
            if( bFound0 ) _fail;
            if( value != 0 ) _fail;
            bFound0 = TRUE;
            break;
        case 1:
            if( bFound1 ) _fail;
            if( value != 1 ) _fail;
            bFound1 = TRUE;
            break;
        case 2:
            if( bFound2 ) _fail;
            if( value != 2 ) _fail;
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

    CMapIntToInt::CPair *pPair = map.PLookup( 0 );
    if( pPair == NULL ) _fail;
    if( pPair->key != 0 ) _fail;
    if( pPair->value != 0 ) _fail;

    bFound0 = FALSE;
    bFound1 = FALSE;
    bFound2 = FALSE;
    pPair = map.PGetFirstAssoc();
    while( pPair != NULL ) {
        switch( pPair->key ) {
        case 0:
            if( bFound0 ) _fail;
            if( pPair->value != 0 ) _fail;
            bFound0 = TRUE;
            break;
        case 1:
            if( bFound1 ) _fail;
            if( pPair->value != 1 ) _fail;
            bFound1 = TRUE;
            break;
        case 2:
            if( bFound2 ) _fail;
            if( pPair->value != 2 ) _fail;
            bFound2 = TRUE;
            break;
        default:
            _fail;
            break;
        }
        pPair = map.PGetNextAssoc( pPair );
    }
    if( !bFound0 ) _fail;
    if( !bFound1 ) _fail;
    if( !bFound2 ) _fail;

    map.RemoveKey( 0 );
    if( map.Lookup( 0, value ) ) _fail;

    CMapIntToInt smap1;
    CMapIntToInt smap2;
    smap1[0] = 0;
    smap1[1] = 1;
    smap1[2] = 2;
    
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    smap1.Serialize( ar );
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    smap2.Serialize( ar2 );
    ar2.Close();

    if( smap2[0] != 0 ) _fail;
    if( smap2[1] != 1 ) _fail;
    if( smap2[2] != 2 ) _fail;

    _PASS;
}
