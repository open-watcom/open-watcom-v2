#include "stdafx.h"

#define TEST_BYTE       ((BYTE)100)
#define TEST_WORD       ((WORD)1000)
#define TEST_LONG       ((LONG)-100000)
#define TEST_DWORD      ((DWORD)100000)
#define TEST_FLOAT      ((float)1.5f)
#define TEST_DOUBLE     ((double)1.5)
#define TEST_INT        ((int)-1000)
#define TEST_SHORT      ((short)-1000)
#define TEST_CHAR       ((char)'a')
#define TEST_WCHAR_T    ((wchar_t)L'a')
#define TEST_UNSIGNED   ((unsigned)1000)
#define TEST_BOOL       true
#define TEST_ULONGLONG  ((ULONGLONG)0x100000000)
#define TEST_LONGLONG   ((LONGLONG)0x100000000)

int main()
{
    CMemFile file;
    CArchive ar( &file, CArchive::store );
    ar << TEST_BYTE;
    ar << TEST_WORD;
    ar << TEST_LONG;
    ar << TEST_DWORD;
    ar << TEST_FLOAT;
    ar << TEST_DOUBLE;
    ar << TEST_INT;
    ar << TEST_SHORT;
    ar << TEST_CHAR;
    ar << TEST_WCHAR_T;
    ar << TEST_UNSIGNED;
    ar << TEST_BOOL;
    ar << TEST_ULONGLONG;
    ar << TEST_LONGLONG;
    ar.Close();

    file.Seek( 0, CFile::begin );
    CArchive ar2( &file, CArchive::load );

    BYTE by;
    ar2 >> by;
    if( by != TEST_BYTE ) _fail;

    WORD w;
    ar2 >> w;
    if( w != TEST_WORD ) _fail;

    LONG l;
    ar2 >> l;
    if( l != TEST_LONG ) _fail;

    DWORD dw;
    ar2 >> dw;
    if( dw != TEST_DWORD ) _fail;

    float f;
    ar2 >> f;
    if( f != TEST_FLOAT ) _fail;

    double d;
    ar2 >> d;
    if( d != TEST_DOUBLE ) _fail;

    int i;
    ar2 >> i;
    if( i != TEST_INT ) _fail;

    short s;
    ar2 >> s;
    if( s != TEST_SHORT ) _fail;

    char ch;
    ar2 >> ch;
    if( ch != TEST_CHAR ) _fail;

    wchar_t wc;
    ar2 >> wc;
    if( wc != TEST_WCHAR_T ) _fail;

    unsigned u;
    ar2 >> u;
    if( u != TEST_UNSIGNED ) _fail;

    bool b;
    ar2 >> b;
    if( b != TEST_BOOL ) _fail;

    ULONGLONG ull;
    ar2 >> ull;
    if( ull != TEST_ULONGLONG ) _fail;

    LONGLONG ll;
    ar2 >> ll;
    if( ll != TEST_LONGLONG ) _fail;

    ar2.Close();
    
    _PASS;
}
