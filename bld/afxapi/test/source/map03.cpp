#include "stdafx.h"

int main()
{
    CMapPtrToPtr map;

    if( !map.IsEmpty() ) _fail;
    if( map.GetCount() != 0 ) _fail;
    if( map.GetSize() != 0 ) _fail;

    map.SetAt( (void *)0, (void *)0 );
    map.SetAt( (void *)1, (void *)1 );
    map.SetAt( (void *)2, (void *)2 );

    void *value;
    if( !map.Lookup( (void *)0, value ) ) _fail;
    if( value != (void *)0 ) _fail;
    if( !map.Lookup( (void *)1, value ) ) _fail;
    if( value != (void *)1 ) _fail;
    if( !map.Lookup( (void *)2, value ) ) _fail;
    if( value != (void *)2 ) _fail;
    if( map.Lookup( (void *)3, value ) ) _fail;

    POSITION    position = map.GetStartPosition();
    void        *key;
    BOOL        bFound0 = FALSE;
    BOOL        bFound1 = FALSE;
    BOOL        bFound2 = FALSE;
    while( position != NULL ) {
        map.GetNextAssoc( position, key, value );
        switch( (INT_PTR)key ) {
        case 0:
            if( bFound0 ) _fail;
            if( value != (void *)0 ) _fail;
            bFound0 = TRUE;
            break;
        case 1:
            if( bFound1 ) _fail;
            if( value != (void *)1 ) _fail;
            bFound1 = TRUE;
            break;
        case 2:
            if( bFound2 ) _fail;
            if( value != (void *)2 ) _fail;
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

    map.RemoveKey( (void *)0 );
    if( map.Lookup( (void *)0, value ) ) _fail;

    _PASS;
}
