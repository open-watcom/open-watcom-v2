#include "stdafx.h"

int main()
{
    char *teststr = "abcdefg";
    int testlen = strlen( teststr );

    CMemFile file;
    file.Write( teststr, testlen );
    file.Write( teststr, testlen );

    char buf[255];
    file.Seek( 0, CFile::begin );
    if( file.GetLength() != testlen * 2 ) _fail;
    if( file.Read( buf, testlen) != testlen ) _fail;
    if( memcmp( buf, teststr, testlen ) != 0 ) _fail;
    if( file.Read( buf, 255 ) != testlen ) _fail;
    if( memcmp( buf, teststr, testlen ) != 0 ) _fail;
    file.Close();

    _PASS;
}
