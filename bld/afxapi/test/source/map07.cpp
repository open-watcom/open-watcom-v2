#include "stdafx.h"

int main()
{
    CMapStringToString map;

    if( !map.IsEmpty() ) _fail;
    if( map.GetCount() != 0 ) _fail;
    if( map.GetSize() != 0 ) _fail;

    map.SetAt( _T("0"), _T(" ") );
    map.SetAt( _T("1"), _T("A") );
    map.SetAt( _T("2"), _T("B") );

    CString value;
    if( !map.Lookup( _T("0"), value ) ) _fail;
    if( value != _T(" ") ) _fail;
    if( !map.Lookup( _T("1"), value ) ) _fail;
    if( value != _T("A") ) _fail;
    if( !map.Lookup( _T("2"), value ) ) _fail;
    if( value != _T("B") ) _fail;
    if( map.Lookup( _T("3"), value ) ) _fail;

    POSITION    position = map.GetStartPosition();
    CString     key;
    BOOL        bFound0 = FALSE;
    BOOL        bFound1 = FALSE;
    BOOL        bFound2 = FALSE;
    while( position != NULL ) {
        map.GetNextAssoc( position, key, value );
        if( key == _T("0") ) {
            if( bFound0 ) _fail;
            if( value != _T(" ") ) _fail;
            bFound0 = TRUE;
        } else if( key == _T("1") ) {
            if( bFound1 ) _fail;
            if( value != _T("A") ) _fail;
            bFound1 = TRUE;
        } else if( key == _T("2") ) {
            if( bFound2 ) _fail;
            if( value != _T("B") ) _fail;
            bFound2 = TRUE;
        } else {
            _fail;
        }
    }
    if( !bFound0 ) _fail;
    if( !bFound1 ) _fail;
    if( !bFound2 ) _fail;

    map.RemoveKey( _T("0") );
    if( map.Lookup( _T("0"), value ) ) _fail;

    CMapStringToString smap1;
    CMapStringToString smap2;
    smap1[_T("0")] = _T(" ");
    smap1[_T("1")] = _T("A");
    smap1[_T("2")] = _T("B");
    
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    smap1.Serialize( ar );
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );
    smap2.Serialize( ar2 );
    ar2.Close();

    if( smap2[_T("0")] != _T(" ") ) _fail;
    if( smap2[_T("1")] != _T("A") ) _fail;
    if( smap2[_T("2")] != _T("B") ) _fail;

    _PASS;
}
