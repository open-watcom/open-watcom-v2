#include "stdafx.h"

int main()
{
    char *teststr = "abcdefg";
    int testlen = strlen( teststr );

    CStdioFile file;
    if( !file.Open( _T("filetest.tmp"), CFile::modeWrite | CFile::modeCreate ) ) {
        _fail;
    }
    file.Write( teststr, testlen );
    file.Close();

    char buf[255];
    if( !file.Open( _T("filetest.tmp"), CFile::modeRead ) ) {
        _fail;
    }
    if( file.GetLength() != testlen ) _fail;
    if( file.Read( buf, 255 ) != testlen ) _fail;
    if( memcmp( buf, teststr, testlen ) != 0 ) _fail;
    file.Close();

    CFile::Remove( _T("filetest.tmp") );

    _PASS;
}
