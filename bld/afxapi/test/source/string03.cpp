#include "stdafx.h"

int main()
{
    CString str = _T("aaaaBBBB");

    CString strWork = str;
    strWork.MakeLower();
    if( strWork != _T("aaaabbbb") ) _fail;
    strWork.MakeUpper();
    if( strWork != _T("AAAABBBB") ) _fail;
    strWork.MakeReverse();
    if( strWork != _T("BBBBAAAA") ) _fail;
    strWork.Truncate( 4 );
    if( strWork != _T("BBBB") ) _fail;

    strWork.Empty();
    if( strWork != _T("") ) _fail;
    if( strWork.GetLength() != 0 ) _fail;
    if( !strWork.IsEmpty() ) _fail;
    if( str.IsEmpty() ) _fail;
    strWork.Append( str );
    strWork.Append( _T("AAA"), 2 );
    strWork.Append( _T("bb") );
    strWork.AppendChar( _T('A') );
    if( strWork != _T("aaaaBBBBAAbbA") ) _fail;

    if( str.Left( 4 ) != _T("aaaa") ) _fail;
    if( str.Right( 4 ) != _T("BBBB") ) _fail;
    if( str.Mid( 2 ) != _T("aaBBBB") ) _fail;
    if( str.Mid( 2, 4 ) != _T("aaBB") ) _fail;
    if( str.Find( _T("aB") ) != 3 ) _fail;
    if( str.Find( _T('B') ) != 4 ) _fail;
    if( str.Find( _T('X') ) >= 0 ) _fail;

    strWork.Format( _T("%d"), 100 );
    if( strWork != _T("100") ) _fail;
    strWork.AppendFormat( _T("%d"), 200 );
    if( strWork != _T("100200") ) _fail;
    strWork.FormatMessage( _T("%1 %2 %1"), _T("AA"), _T("BB") );
    if( strWork != _T("AA BB AA") ) _fail;

    LPTSTR lpsz = strWork.GetBuffer( 10 );
    _tcscpy( lpsz, _T("aaaa") );
    strWork.ReleaseBuffer();
    if( strWork != _T("aaaa") ) _fail;

    _PASS;
}
