#include "stdafx.h"

int main()
{
    CString str1 = _T("aaaa");
    CString str2 = _T("bbbb");

    CString test1( str1 );
    if( test1 != str1 ) _fail;
    CString test2( "aaaa" );
    if( test2 != str1 ) _fail;
    CString test3( L"aaaa" );
    if( test3 != str1 ) _fail;
    CString test4( 'a', 4 );
    if( test4 != str1 ) _fail;
    CString test5( L'a', 4 );
    if( test5 != str1 ) _fail;
    CString test6( "aaaaaa", 4 );
    if( test6 != str1 ) _fail;
    CString test7( L"aaaaaa", 4 );
    if( test7 != str1 ) _fail;

    if( !(str1 < str2) ) _fail;
    if( !(str1 < _T("bbbb")) ) _fail;
    if( !(_T("aaaa") < str2) ) _fail;
    if( str1 > str2 ) _fail;
    if( str1 > _T("bbbb") ) _fail;
    if( _T("aaaa") > str2 ) _fail;
    if( !(str1 <= str2) ) _fail;
    if( !(str1 <= _T("bbbb")) ) _fail;
    if( !(_T("aaaa") <= str2) ) _fail;
    if( str1 >= str2 ) _fail;
    if( str1 >= _T("bbbb") ) _fail;
    if( _T("aaaa") >= str2 ) _fail;

    if( str1.Collate( str2 ) >= 0 ) _fail;
    if( str1.CollateNoCase( str2 ) >= 0 ) _fail;
    if( str1.Compare( str2 ) >= 0 ) _fail;
    if( str1.CompareNoCase( str2 ) >= 0 ) _fail;
    
    _PASS;
}
