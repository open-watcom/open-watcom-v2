#include "stdafx.h"

int main()
{
    LPCTSTR teststr = _T("abcdefg");
    int testlen = _tcslen( teststr );

    CStdioFile file;
    if( !file.Open( _T("filetest.tmp"), CFile::modeWrite | CFile::modeCreate ) ) {
        _fail;
    }
    file.WriteString( teststr );
    file.Close();

    CString str;
    if( !file.Open( _T("filetest.tmp"), CFile::modeRead ) ) {
        _fail;
    }
    if( !file.ReadString( str ) ) _fail;
    if( str != teststr ) _fail;

    TCHAR buf[255];
    file.Seek( 0, CFile::begin );
    if( file.ReadString( buf, 255 ) == NULL ) _fail;
    if( _tcscmp( buf, teststr ) != 0 ) _fail;
    file.Close();

    CFile::Remove( _T("filetest.tmp") );

    _PASS;
}
